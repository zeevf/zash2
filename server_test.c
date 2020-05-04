//
// Created by user on 5/3/20.
//

#include <stdbool.h>

#include "common.h"
#include "socket/socket.h"


int main(void)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct SOCKET_syn_context *context = NULL;
    size_t data_len = 256;
    uint8_t data[256] = {0};
    char ip[256] = {0};

    status = SOCKET_syn_create("lo", &context);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    while (true) {

        data_len = 256;
        ZERO_MEMORY(data, data_len);

        status = SOCKET_syn_receive(context, 7891, &data_len, data, ip);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        printf("%s", data);
        printf("\n");

    }

lbl_cleanup:

    (void)SOCKET_syn_destroy(context);

    return status;

}