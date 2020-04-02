/**
 * @brief This module scan for commands. the commands are scanned by checking for
 *        process with the magic command "genadgalili".
 * @aouther Z.F
 * @date 19/3/2020
 */

/** Headers ***************************************************/
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "utils/utils.h"
#include "common.h"

#include "scanner.h"
#include "scanner_internal.h"


/** Functions ************************************************/
enum zash_status scanner_is_scanned(int pid, struct VECTOR_context *scanned_pids, bool *is_scanned)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int *current_pid = NULL;
    bool temp_is_scanned = false;
    size_t i = 0;

    /* Check for valid parameters */
    ASSERT(NULL != scanned_pids);
    ASSERT(NULL != is_scanned);

    /* check that the given process is'nt already scanned. */
    for (i = 0; i < scanned_pids->array_size; ++i) {

        /* If the current pid is the same as the requested pid, this process was already scanned. */
        current_pid = scanned_pids->array[i];
        if (*current_pid == pid) {
            temp_is_scanned = true;
            break;
        }
    }

    /* Transfer Ownership */
    *is_scanned = temp_is_scanned;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

    return status;

}


enum zash_status scanner_mark_as_scanned(int pid, struct VECTOR_context *scanned_pids)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int *allocated_pid = NULL;

    /* Check for valid parameters */
    ASSERT(NULL != scanned_pids);

    /* Allocate memory to store the pid in. */
    allocated_pid = HEAPALLOCZ(sizeof(*allocated_pid));
    if (NULL == allocated_pid) {
        status = ZASH_STATUS_SCANNER_MARK_AS_VISITED_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    *allocated_pid = pid;

    /* Add the pid to the vector of scanned pids */
    status = VECTOR_push(scanned_pids, allocated_pid);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }
    allocated_pid = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(allocated_pid);

    return status;

}


enum zash_status scanner_get_command_line(int pid,
                                          size_t max_command_line,
                                          char *command_line,
                                          size_t *command_line_len)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    FILE *cmd_file = NULL;
    char *cmd_path = NULL;
    size_t temp_command_line_len = 0;
    int snprintf_return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    ASSERT(NULL != command_line);

    /* Find the length of the path of the file contains the command line */
    snprintf_return_value = snprintf(NULL, 0, SCANNER_CMD_PATH_FORMAT, pid);
    if (C_STANDARD_FAILURE_VALUE == snprintf_return_value) {
        status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_SNPRINTF_FOR_SIZE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }
    snprintf_return_value++;

    /* Allocate memory for the path */
    cmd_path = HEAPALLOCZ(snprintf_return_value * sizeof(*cmd_path));
    if (NULL == cmd_path) {
        status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble the path of the file contains the command line */
    snprintf_return_value = snprintf(cmd_path, snprintf_return_value, SCANNER_CMD_PATH_FORMAT, pid);
    if (C_STANDARD_FAILURE_VALUE == snprintf_return_value) {
        status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_SNPRINTF_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the file contains the command line */
    cmd_file = fopen(cmd_path, FOPEN_READ_ONLY);
    if (NULL == cmd_file) {

        /* If fopen failed because of the file does not exist, that mean that the process
         * no longer exist. this is a different error */
        if (ENOENT == errno) {
            status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_FILE;

        }
        else {
            status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FOPEN_FAILED;
            DEBUG_PRINT("status: %d", status);
        }

        goto lbl_cleanup;
    }

    /* Read the command line. */
    errno = ERRNO_SUCCESS;
    temp_command_line_len = fread(command_line, sizeof(*command_line), max_command_line, cmd_file);
    if (ZASH_STATUS_SUCCESS != errno) {
        status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FREAD_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Check if any command line read */
    if (0 == temp_command_line_len) {
        status = ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_LINE;
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *command_line_len = temp_command_line_len;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    FCLOSE(cmd_file);
    HEAPFREE(cmd_path);

    return status;
}


enum zash_status scanner_is_required(const char *command_name, bool *is_required)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    bool temp_is_required = false;
    char *command_name_dup = NULL;
    char *command_name_base = NULL;

    /* Check for valid parameters */
    ASSERT(NULL != is_required);
    ASSERT(NULL != command_name);

    /* copy the command so we could use basename on it */
    command_name_dup = strdup(command_name);
    if (NULL == command_name_dup) {
        status = ZASH_STATUS_SCANNER_IS_REQUIRED_STRDUP_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the name of the command, without it's full path */
    command_name_base = basename(command_name_dup);

    /* Check if the command is of a process we need to gather data about */
    temp_is_required = (0 == strcmp(command_name_base, SCANNER_REQUIRED_COMMAND));

    /* Transfer Ownership */
    *is_required = temp_is_required;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(command_name_dup);

    return status;
}


enum zash_status scanner_extract_data(int pid,
                                      char *command_line,
                                      size_t command_line_len,
                                      struct SCANNER_data **data)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SCANNER_data *new_data = NULL;
    char *current_arg = NULL;
    char *current_arg_dup = NULL;
    bool is_first_arg = true;

    /* Check for valid parameters */
    ASSERT(NULL != data);

    /* Allocate memory for new data */
    new_data = HEAPALLOCZ(sizeof(*new_data));
    if (NULL == new_data) {
        status = ZASH_STATUS_SCANNER_EXTRACT_DATA_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* get the niceness of the process - it is the command id */
    errno = ERRNO_SUCCESS;
    new_data->id = getpriority(PRIO_PROCESS, pid);
    if ((C_STANDARD_FAILURE_VALUE == new_data->id) && ERRNO_SUCCESS != errno) {
        status = ZASH_STATUS_SCANNER_EXTRACT_DATA_GETPRIORITY_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a new vector to contain the command's args */
    status = VECTOR_create(&new_data->argv);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    current_arg = command_line;

    /* extract commands line argument from the command line into a vector */
    while (command_line_len > 0) {

        /* dont parse the first argument in the command line - it is the command name */
        if (false == is_first_arg) {

            /* Copy the argument */
            current_arg_dup = strdup(current_arg);
            if (NULL == current_arg_dup) {
                status = ZASH_STATUS_SCANNER_EXTRACT_DATA_STRDUP_FAILED;
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }

            /* Add it to the Vector of arguments */
            status = VECTOR_push(new_data->argv, current_arg_dup);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }

            current_arg_dup = NULL;
        }
        else {
            /* The next argument is not first */
            is_first_arg = false;
        }

        /* Update the length of remains data to extract */
        command_line_len -= strlen(current_arg) + 1;
        current_arg += strlen(current_arg) + 1;

    }

    /* Transfer Ownership */
    *data = new_data;
    new_data = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(current_arg_dup);

    if (NULL != new_data) {
        temp_status = SCANNER_free_data(new_data);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;
}


enum zash_status scanner_scan_by_pid(struct SCANNER_context *context, int pid)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char command_line[SCANNER_MAX_COMMAND_LINE_LENGTH] = {0};
    size_t command_line_len = 0;
    struct SCANNER_data *new_data = NULL;
    bool is_scanned = false;
    bool is_required = false;

    /* Check for valid parameters */
    ASSERT(NULL != context);

    /* Check if process was already scanned */
    status = scanner_is_scanned(pid, context->scanned_pids, &is_scanned);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    if (false == is_scanned) {

        /* Get the command line of the process. */
        status = scanner_get_command_line(pid,
                                          SCANNER_MAX_COMMAND_LINE_LENGTH,
                                          command_line,
                                          &command_line_len);
        if (ZASH_STATUS_SUCCESS != status) {
            goto lbl_cleanup;
        }

        /* Check if the data of the process is required */
        status = scanner_is_required(command_line, &is_required);
        if (ZASH_STATUS_SUCCESS != status &&
            ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FOPEN_FAILED != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        if (true == is_required) {

            /* Get the data of the process */
            status = scanner_extract_data(pid, command_line, command_line_len, &new_data);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }

            /* Add the data to the vector of data found */
            status = VECTOR_push(context->data, new_data);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }

            /* Mark this pid as scanned */
            status = scanner_mark_as_scanned(pid, context->scanned_pids);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }
        }
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status scanner_scan_file(const char *file, struct SCANNER_context *context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    unsigned long strtoul_return_value = 0;
    char *endptr = NULL;

    /* Check for valid parameters */
    ASSERT(NULL != file);
    ASSERT(NULL != context);

    /* convert the name of the file to number */
    errno = ERRNO_SUCCESS;
    strtoul_return_value = strtoul(file, &endptr, DECIMAL_BASE);
    if (ERRNO_SUCCESS != errno) {
        status = ZASH_STATUS_SCANNER_SCAN_FILE_STRTOUL_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* if the name of the file is a number - that number is a pid of a process. */
    if (endptr != file) {
        /* Scan the found process for data */
        status = scanner_scan_by_pid(context, (int) strtoul_return_value);

        /* If the data of the process want exist, we want to continue scanning for processes. */
        if ((ZASH_STATUS_SUCCESS != status) &&
            (ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_FILE != status) &&
            (ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_LINE != status)) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status SCANNER_free_data(struct SCANNER_data *data)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    /* Free the vector contains arguments */
    if (NULL != data) {
        VECTOR_destroy(data->argv, UTILS_free);
    }
    HEAPFREE(data);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

    return status;

}


enum zash_status SCANNER_create(struct SCANNER_context **context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SCANNER_context *temp_context = NULL;

    /* Check for valid parameters */
    if (NULL == context) {
        status = ZASH_STATUS_SCANNER_CREATE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for new context */
    temp_context = HEAPALLOCZ(sizeof(*temp_context));
    if (NULL == temp_context) {
        status = ZASH_STATUS_SCANNER_CREATE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create vector for scanned pids */
    status = VECTOR_create(&temp_context->scanned_pids);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *context = temp_context;
    temp_context = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (NULL != temp_context) {
        temp_status = SCANNER_destroy(temp_context);
        if (ZASH_STATUS_SUCCESS != temp_status) {
            ZASH_UPDATE_STATUS(status, temp_status);
        }
    }

    return status;
}


enum zash_status SCANNER_scan(struct SCANNER_context *context, struct VECTOR_context **data)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == data)) {
        status = ZASH_STATUS_SCANNER_SCAN_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create vector for found data */
    status = VECTOR_create(&context->data);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Scan all processes by scanning the /proc directory, that contains all processes pids */
    status = UTILS_iter_dir(SCANNER_PROC_PATH,
                            (UTILS_iter_dir_callback_t) scanner_scan_file,
                            context);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *data = context->data;
    context->data = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (NULL != context->data) {
        temp_status = VECTOR_destroy(context->data, (VECTOR_free_func_t) SCANNER_free_data);
        if (ZASH_STATUS_SUCCESS != temp_status) {
            DEBUG_PRINT("status: %d", status);
            ZASH_UPDATE_STATUS(status, temp_status);
        }
    }

    return status;
}


enum zash_status SCANNER_destroy(struct SCANNER_context *context)
{

    enum zash_status status = ZASH_STATUS_SUCCESS;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    if (NULL != context) {

        if (NULL != context->scanned_pids) {
            temp_status = VECTOR_destroy(context->scanned_pids, UTILS_free);
            ZASH_UPDATE_STATUS(status, temp_status);
        }

        if (NULL != context->data) {
            temp_status = VECTOR_destroy(context->data, (VECTOR_free_func_t) SCANNER_free_data);
            ZASH_UPDATE_STATUS(status, temp_status);
        }

        HEAPFREE(context);
    }

    return status;
}