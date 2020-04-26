/**
 * @brief This module run commands. the command to run is determined by an id.
 * @aouther Z.F
 * @date 19/3/2020
 */

/** Headers ***************************************************/
#include <unistd.h>
#include <stdbool.h>

#include "utils/utils.h"
#include "common.h"
#include "config.h"

#include "runner.h"
#include "runner_internal.h"


/** Functions ***********************************************/
enum zash_status runner_copy_log(const char *log_dir, const char *log_file, const char *dir)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char new_path[PATH_MAX] = {0};
    char log_path[PATH_MAX] = {0};
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    ASSERT(NULL != log_file);
    ASSERT(NULL != dir);

    /* Assemble path of the log file to copy */
    return_value = snprintf(new_path, PATH_MAX, RUNNER_DIR_FORMAT, dir, log_file);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_NEW_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Check that the new path want too long */
    if (return_value >= PATH_MAX) {
        status = ZASH_STATUS_RUNNER_COPY_LOG_NEW_PATH_TOO_LONG;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble path of the file to copy into */
    return_value = snprintf(log_path, PATH_MAX, RUNNER_DIR_FORMAT, log_dir, log_file);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_LOG_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Check that the log path want too long */
    if (return_value >= PATH_MAX) {
        status = ZASH_STATUS_RUNNER_COPY_LOG_LOG_PATH_TOO_LONG;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Copy the log file */
    status = UTILS_copy_file(log_path, new_path);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;

}


enum zash_status runner_copy_logs(int argc, const char *const argv[], pthread_rwlock_t *lock)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    int return_value = C_STANDARD_FAILURE_VALUE;
    bool is_lock_Acquired = false;

    /* Check for valid parameters */
    ASSERT(NULL != argv);

    /* Check for valid parameter count */
    if (argc < RUNNER_COPY_LOGS_PARAMETER_MAX_PARAMETERS) {
        status = ZASH_STATUS_RUNNER_COPY_LOGS_NOT_ENOUGH_ARGUMENTS;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create the directory to copy logs into */
    status = UTILS_create_dirs(argv[RUNNER_COPY_LOGS_PARAMETER_DESTINATION_DIR]);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Acquire lock for reading logs */
    return_value = pthread_rwlock_rdlock(lock);
    if (C_STANDARD_SUCCESS_VALUE != return_value) {
        status = ZASH_STATUS_RUNNER_COPY_LOGS_PTHREAD_RWLOCK_RDLOCK_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }
    is_lock_Acquired = true;

    /* For each log file in the logs file directory, copy it into the new directory. */
    status = UTILS_iter_dir(
            ZASH_LOGS_PATH,
            (UTILS_iter_dir_callback_t)runner_copy_log,
            (void *)argv[RUNNER_COPY_LOGS_PARAMETER_DESTINATION_DIR]);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (is_lock_Acquired) {
        return_value = pthread_rwlock_unlock(lock);
        if (C_STANDARD_SUCCESS_VALUE != return_value) {
            temp_status = ZASH_STATUS_RUNNER_COPY_LOGS_PTHREAD_RWLOCK_UNLOCK_FAILED;
            ZASH_UPDATE_STATUS(status, temp_status);
        }
    }

    return status;

}


enum zash_status runner_change_color(int argc, const char *const argv[])
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char color = 0;
    const char *color_parameter = NULL;
    char command_to_print[RUNNER_COLOR_COMMAND_LEN] = {0};
    ssize_t write_return_value = C_STANDARD_FAILURE_VALUE;
    int snprintf_return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    ASSERT(NULL != argv);

    /* Check for valid parameter count */
    if (argc < RUNNER_CHANGE_COLOR_PARAMETER_MAX_PARAMETERS) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_NOT_ENOUGH_ARGUMENTS;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* get the new color parameter */
    color_parameter = argv[RUNNER_CHANGE_COLOR_PARAMETER_NEW_COLOR];

    /* get the first character of the parameter */
    color = *color_parameter;

    /* Check if the character represents a color */
    if (RUNNER_IS_VALID_COLOR(color)) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_WRONG_COLOR;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble Terminal command for changing color */
    snprintf_return_value = snprintf(
            command_to_print, RUNNER_COLOR_COMMAND_LEN, RUNNER_COLOR_COMMAND, color);
    if (C_STANDARD_FAILURE_VALUE == snprintf_return_value) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_SNPRINTF_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Check that the command was not truncated */
    if (snprintf_return_value >= RUNNER_COLOR_COMMAND_LEN) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_COMMAND_TRUNCATED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Write the command to change the color */
    write_return_value = write(STDOUT_FILENO, command_to_print, snprintf_return_value);
    if (write_return_value < snprintf_return_value) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_WRITE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;

}


enum zash_status
RUNNER_run(enum RUNNER_command_id command_id, int argc, const char *const *argv, pthread_rwlock_t *lock)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    /* Check for valid parameters */
    if (NULL == argv) {
        status = ZASH_STATUS_RUNNER_RUN_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Run the command. */
    switch (command_id) {

    case RUNNER_CHANGE_COLOR_COMMAND_ID:
        /* Change the color of the terminal. */
        status = runner_change_color(argc, argv);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        break;

    case RUNNER_COPY_LOGS_COMMAND_ID:
        /* Copy the log files into a new directory. */
        status = runner_copy_logs(argc, argv, lock);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        break;

    default:

        /* No command was given */
        status = ZASH_STATUS_RUNNER_RUN_NO_COMMANDS;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}