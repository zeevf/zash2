/**
 * @brief This module make a process become a daemon process.
 * @aouther Z.F
 * @date 03/05/2020
 */

/** Headers ***************************************************/
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"

#include "daemon.h"
#include "daemon_internal.h"


/** Functions ************************************************/
enum zash_status daemon_become_new_process(void)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    pid_t pid = INVALID_PROCESS_ID;

    /* switch into new process */
    pid = fork();
    if (C_STANDARD_FAILURE_VALUE == pid) {
        status = ZASH_STATUS_DAEMON_BECOME_NEW_PROCESS_FORK_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Exit the parent process */
    if (CHILD_PROCESS != pid) {
        exit(EXIT_SUCCESS);
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status daemon_become_session_leader(void)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    pid_t setsid_return_value = C_STANDARD_FAILURE_VALUE;

    /* switch into new process */
    status = daemon_become_new_process();
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a new session */
    setsid_return_value = setsid();
    if (C_STANDARD_FAILURE_VALUE == setsid_return_value) {
        status = ZASH_STATUS_DAEMON_BECOME_SESSION_LEADER_SETSID_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status daemon_close_all_files(void)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    long int max_opened = C_STANDARD_FAILURE_VALUE;
    size_t i = 0;

    /* Find the maximum amount of open files */
    max_opened = sysconf(_SC_OPEN_MAX);
    if (C_STANDARD_FAILURE_VALUE == max_opened) {
        status = ZASH_STATUS_DAEMON_CLOSE_ALL_FILES_SYSCONF_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* For each fd that might be opened, close it. */
    for (i = 0; i < max_opened; ++i) {
        /* Close the fd. we are not using CLOSE macro to avoid setting i value. */
        (void)close(i);
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status daemon_open_standard_files(char *path)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int null_fd = INVALID_FILE_DESCRIPTOR;
    int fd = INVALID_FILE_DESCRIPTOR;

    /* Check for valid parameters */
    ASSERT(NULL != path);

    /* Open the null file */
    null_fd = open(path, O_RDWR);
    if (INVALID_FILE_DESCRIPTOR == null_fd) {
        status = ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_OPEN_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* open duplicate the null file into stdin */
    fd = dup2(null_fd, STDIN_FILENO);
    if (INVALID_FILE_DESCRIPTOR == fd) {
        status = ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_DUP_STDIN_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* open duplicate the null file into stdout */
    fd = dup2(null_fd, STDOUT_FILENO);
    if (INVALID_FILE_DESCRIPTOR == fd) {
        status = ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_DUP_STDOUT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* open duplicate the null file into stderr */
    fd = dup2(null_fd, STDERR_FILENO);
    if (INVALID_FILE_DESCRIPTOR == fd) {
        status = ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_DUP_STDERR_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    /* Close the null fd if it is not a standard file */
    if ((STDIN_FILENO != null_fd) && (STDOUT_FILENO != null_fd) && (STDERR_FILENO != null_fd)) {
        CLOSE(null_fd);
    }

    return status;
}


enum zash_status DAEMON_daemonize(void)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int chdir_return_value = C_STANDARD_FAILURE_VALUE;

    /* Set the umask to remove any restrictions from the file creating mode */
    (void)umask(DAEMON_UMASK_NO_RESTRICTION);

    /* Change the current directory to root directory */
    chdir_return_value = chdir(ROOT_DIRECTORY);
    if (C_STANDARD_FAILURE_VALUE == chdir_return_value) {
        status = ZASH_STATUS_DAEMON_DAEMONIZE_CHDIR_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Switch into a new session leader file. */
    status = daemon_become_session_leader();
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Switch into a new process */
    status = daemon_become_new_process();
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Close all opened files */
    status = daemon_close_all_files();
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the standard files as null file */
    status = daemon_open_standard_files(DAEMON_NULL_FILE_PATH);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}
