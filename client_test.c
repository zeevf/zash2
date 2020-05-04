//
// Created by user on 5/3/20.
//

#include <stdbool.h>

#include "common.h"
#include "socket/socket.h"

int main(void) {
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct SOCKET_syn_context *context = NULL;
    size_t data_len = 256;
    uint8_t data [256] = {0};
    char ip [256] = {0};

    status = SOCKET_syn_create("lo", &context);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    while(true) {

        ZERO_MEMORY(data, data_len);
        scanf("%s", data);

        status = SOCKET_syn_send(context, "127.0.0.1", 7890, data, strlen(data) + 1);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        ZERO_MEMORY(data, data_len);
        scanf("%s", data);

        status = SOCKET_syn_send(context, "127.0.0.1", 7891, data, strlen(data) + 1);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

    }

lbl_cleanup:

    (void)SOCKET_syn_destroy(context);

    return status;

}