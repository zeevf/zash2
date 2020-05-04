//
// Created by user on 5/4/20.
//

#ifndef ZASH_CLIENT_H
#define ZASH_CLIENT_H

#include <stdint.h>

#include "zash_status.h"


enum zash_status
CLIENT_run(const char *interface, const char *ip, uint16_t port, uint16_t port_to_connect);


#endif //ZASH_CLIENT_H
