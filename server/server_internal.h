/**
 * @brief Internal header for server.c. This module is a daemon server
 *        that wait for port knocking from a client, let it run commands.
 * @aouther Z.F
 * @date 04/05/2020
 */

#ifndef ZASH_SERVER_INTERNAL_H
#define ZASH_SERVER_INTERNAL_H

/** Headers ***************************************************/
#include "socket/socket.h"
#include "shell/shell.h"

#include "server.h"

/** Constants ***************************************************/
/* The maximum length of an ip4 text. */
#define SERVER_MAX_IP_LENGTH (16)

/**
 * @brief Prepare the server for running - create shell and SOCKET_syn objects and become a daemon.
 *
 * @param [in]     interface           the name of the interface for the server to use.
 *
 * @param [out]    shell               the shell to use for running client commands.
 *
 * @param [out]    socket_context      the socket syn object to listen for port knocking with.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           This function will make this process to become a daemon process.
 *
 */
enum zash_status server_prepare(const char *interface,
                                struct SHELL_context **shell,
                                struct SOCKET_syn_context **socket_context);


/**
 * @brief Get the address to connect to through port knocking.
 *
 * @param [in]     socket_context      the socket syn object to listen for port knocking with.
 *
 * @param [in]     port                the port to listen for port knocking on.
 *
 * @param [out]    port_to_connect      the port to connect to.
 *
 * @param [out]    ip_to_connect        the ip to connect to.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           This function may block until a client will port knock successfully.
 *
 * @note           ip_to_connect must be previously allocated with at least 16 bytes length,
 *                 the maximum size of an ip address.
 */
enum zash_status server_get_address_to_connect(struct SOCKET_syn_context *context,
                                               uint16_t port,
                                               uint16_t *port_to_connect,
                                               char *ip_to_connect);


/**
 * @brief Run a shell and attach it standard io to a socket.
 *
 * @param [in]     shell               the shell to run.
 *
 * @param [in]     fd_socket           the socket to connect shell to.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           this function will return only when the shell or the connection is closed.
 */
enum zash_status server_run_shell(struct SHELL_context *shell, int fd_socket);


#endif //ZASH_SERVER_INTERNAL_H
