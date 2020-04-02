/**
 * @brief This module run commands. the command to run is determined by an id.
 * @aouther Z.F
 * @date 19/3/2020
 */

/** Headers ***************************************************/
#include <unistd.h>
#include <pthread.h>

#include "utils/utils.h"
#include "common.h"
#include "config.h"

#include "runner.h"
#include "runner_internal.h"


/** Functions ***********************************************/
enum zash_status runner_copy_log(const char *log_file, const char *dir)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char new_path[PATH_MAX] = {0};
    char log_path[PATH_MAX] = {0};
    pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
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

    /* Assemble path of the file to copy into */
    return_value = snprintf(log_path, PATH_MAX, RUNNER_DIR_FORMAT, ZASH_LOGS_PATH, log_file);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_LOG_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Acquire lock for reading logs */
    return_value = pthread_rwlock_rdlock(&lock);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_RUNNER_COPY_LOG_PTHREAD_RWLOCK_RDLOCK_FAILED;
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


enum zash_status runner_copy_logs(int argc, char *const argv[])
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

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

    /* For each log file in the logs file directory, copy it into the new directory. */
    status = UTILS_iter_dir(ZASH_LOGS_PATH,
                            (UTILS_iter_dir_callback_t) runner_copy_log,
                            argv[RUNNER_COPY_LOGS_PARAMETER_DESTINATION_DIR]);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;

}


enum zash_status runner_change_color(int argc, char *const argv[])
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char color = 0;
    char command_to_print[sizeof(RUNNER_COLOR_COMMAND)] = {0};
    size_t write_len = 0;
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

    /* get the first character of the parameter */
    color = *argv[RUNNER_CHANGE_COLOR_PARAMETER_NEW_COLOR];

    /* Check if the character represents a color */
    if (RUNNER_IS_VALID_COLOR(color)) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_WRONG_COLOR;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble Terminal command for changing color */
    snprintf_return_value = snprintf(command_to_print,
                                     sizeof(command_to_print),
                                     RUNNER_COLOR_COMMAND,
                                     color);
    if (C_STANDARD_FAILURE_VALUE == snprintf_return_value) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_SNPRINTF_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Write the command to change the color */
    write_len = strlen(command_to_print);
    write_return_value = write(STDOUT_FILENO,
                               command_to_print,
                               write_len);
    if (write_return_value < write_len) {
        status = ZASH_STATUS_RUNNER_CHANGE_COLOR_WRITE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;

}


enum zash_status RUNNER_run(int command_id, int argc, char *const argv[])
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    /* Run the command. */
    switch (command_id) {

    case RUNNER_CHANGE_COLOR_ID:
        /* Change the color of the terminal. */
        status = runner_change_color(argc, argv);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        break;

    case RUNNER_COPY_LOGS_ID:
        /* Copy the log files into a new directory. */
        status = runner_copy_logs(argc, argv);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        break;

    default:
        /* No command was given */
        break;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}