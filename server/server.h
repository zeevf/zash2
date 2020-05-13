/**
 * @brief Header for server.c. This module is a daemon server
 *        that wait for port knocking from a client, let it run commands.
 * @aouther Z.F
 * @date 04/05/2020
 */


#ifndef ZASH_SERVER_H
#define ZASH_SERVER_H

/** Headers ***************************************************/
#include <stdint.h>

#include "zash_status.h"

/** Functions ************************************************/
/**
 * @brief Run a daemon server that listen for clients an let them run commands.
 *
 * @param [in]     port                the port to listen on for port knocking from clients.
 *
 * @param [in]     interface           the name of the interface to use.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           This function will make this process to become a daemon process.
 *
 * @note           If this function succeed, it does not return.
 *
 */
enum zash_status SERVER_run(uint16_t port, const char *interface);

#endif //ZASH_SERVER_H
