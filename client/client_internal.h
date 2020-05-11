/**
 * @brief Internal header for client.c. This module run a client that run commands on a server.
 * @aouther Z.F
 * @date 07/05/2020
 */

#ifndef ZASH_CLIENT_INTERNAL_H
#define ZASH_CLIENT_INTERNAL_H

/** Headers ***************************************************/
#include "client.h"

/** Constants *************************************************/
/* The maximum length of data to transfer between socket and terminal at once */
#define MAX_TRANSFER_LENGTH (4096)

/** Functions *************************************************/
/**
 * @brief Use port knocking to connect to the server.
 *
 * @param [in]           interface        the name of the interface to use.
 *
 * @param [in]           ip               the ip address of the distant server.
 *
 * @param [in]           port             the port to send port knock to.
 *
 * @param [in]           listen_port      the port to listen on with tcp after the port knocking.
 *
 * @param [out]          fd_socket        the socket connected to the server.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 dont use this function if the server is'nt listening for port knocking;
 *                       it might block forever.
 *
 */
enum zash_status client_port_knock(const char *interface,
                                   const char *ip,
                                   uint16_t port,
                                   uint16_t listen_port,
                                   int *fd_socket);


/**
 * @brief pass data between standard io and a fd.
 *
 * @param [in]          fd           the fd to connect to the terminal.
 *
 * @return              return value indicating an error may returned.
 *
 * @note                This function will return only when the fd or the standard io is closed.
 *
 */
enum zash_status client_connect_terminal_to_fd(int fd);


#endif //ZASH_CLIENT_INTERNAL_H
