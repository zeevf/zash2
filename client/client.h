/**
 * @brief Header for client.c. This module run a client that run commands on a server.
 * @aouther Z.F
 * @date 04/05/2020
 */

#ifndef ZASH_CLIENT_H
#define ZASH_CLIENT_H

/** Headers ***************************************************/
#include <stdint.h>

#include "zash_status.h"

/** Functions *************************************************/
/**
 * @brief Run a client that connect to a distant server and run commands on it.
 *
 * @param [in]           interface        the name of the interface to use.
 *
 * @param [in]           ip               the ip address of the distant server.
 *
 * @param [in]           port             the port to send port knock to.
 *
 * @param [in]           port_to_connect  the port to connect to with tcp after the port knocking.
 *
 * @return              return value indicating an error may returned.
 *
 * @note                 dont use this function if the server is'nt listening for port knocking;
 *                       it might block forever.
 *
 * @note                 if this function succeed, it returns after the distant shell is closed.
 */
enum zash_status
CLIENT_run(const char *interface, const char *ip, uint16_t port, uint16_t port_to_connect);


#endif //ZASH_CLIENT_H
