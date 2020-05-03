//
// Created by user on 5/3/20.
//

#ifndef ZASH_SOCKET_H
#define ZASH_SOCKET_H

#include <stdlib.h>
#include <stdint.h>

#include "zash_status.h"

struct SOCKET_syn_context;


enum zash_status SOCKET_syn_create(const char *interface, struct SOCKET_syn_context **context);


enum zash_status SOCKET_syn_send(struct SOCKET_syn_context *context,
                                 const char *ip,
                                 uint16_t port,
                                 const uint8_t *data,
                                 size_t data_len
);


enum zash_status SOCKET_syn_receive(struct SOCKET_syn_context *context,
                                    uint16_t listen_port,
                                    size_t *data_len,
                                    uint8_t *data,
                                    char *src_ip
);

enum zash_status SOCKET_syn_destroy(struct SOCKET_syn_context *context);

#endif //ZASH_SOCKET_H
