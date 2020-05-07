/**
 * @brief This module has some common functions.
 * @aouther Z.F
 * @date 19/3/2020
 */

/** Headers ***************************************************/
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>

#include "common.h"

#include "utils.h"
#include "utils_internal.h"


/** Functions ************************************************/
enum zash_status utils_get_dir_by_index(const char *path, size_t index, size_t dir_max, char *dir)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    const char *end_of_dir = path;
    size_t dir_len = 0;
    size_t i = 0;

    /* Check for valid parameters */
    ASSERT (NULL != path);
    ASSERT (NULL != dir);

    /* Ignore first dir sign */
    if (*end_of_dir == UTILS_DIRECTORY_SIGN) {
        end_of_dir++;
    }

    /* Iterate path until getting the requested directory. */
    for (i = 0; i <= index; ++i) {
        if (NULL == end_of_dir) {
            status = ZASH_STATUS_UTILS_CGET_DIR_BY_INDEX_INVALID_INDEX;
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* find the next dir sign */
        end_of_dir = strchr(end_of_dir, UTILS_DIRECTORY_SIGN);

        /* Move to the sign after the dir sign so we can find the next one after it */
        if (NULL != end_of_dir) {
            end_of_dir++;
        }
    }

    /* If no dir sign found, we need the whole path */
    if (NULL == end_of_dir) {
        dir_len = strlen(path);
    }
        /* Get the length from the start of path to the directory sign found */
    else {
        dir_len = end_of_dir - path;
    }

    /* Make sure to not write over max size */
    dir_len = MIN(dir_len, dir_max);

    /* Copy the path found */
    strncpy(dir, path, dir_len);
    dir[dir_len] = NULL_TERMINATOR;


    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status UTILS_create_dirs(const char *path)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char current_dir[PATH_MAX] = {0};
    struct stat stat_buffer = {0};
    size_t path_len = 0;
    size_t current_dir_len = 0;
    size_t i = 0;
    bool is_dir = false;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if (NULL == path) {
        status = ZASH_STATUS_UTILS_CREATE_DIRS_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create the directories is the path */
    path_len = strlen(path);
    while (current_dir_len < path_len) {

        /* get the current directory to create */
        status = utils_get_dir_by_index(path, i, PATH_MAX, current_dir);
        if (ZASH_STATUS_SUCCESS != status) {
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }

        /* Check if the file is already exist */
        errno = ERRNO_SUCCESS;
        return_value = stat(current_dir, &stat_buffer);
        if ((C_STANDARD_FAILURE_VALUE == return_value) && (ENOENT != errno)) {
            status = ZASH_STATUS_UTILS_CREATE_DIRS_STAT_FAILED;
            DEBUG_PRINT("status: %d, current_dir: %s", status, current_dir);
            goto lbl_cleanup;
        }

        if (ERRNO_SUCCESS == errno) {
            /* The file is already exist. Check if it a directory */
            is_dir = S_ISDIR(stat_buffer.st_mode);

            /* If the file is not a directory, we can't use tha given path. */
            if (false == is_dir) {
                status = ZASH_STATUS_UTILS_CREATE_DIRS_FILE_EXIST;
                DEBUG_PRINT("status: %d, current_dir: %s", status, current_dir);
                goto lbl_cleanup;
            }
        }
        else {
            /* Create the directory. */
            return_value = mkdir(current_dir, UTILS_NEW_DIR_PERM);
            if ((C_STANDARD_FAILURE_VALUE == return_value)) {
                status = ZASH_STATUS_UTILS_CREATE_DIRS_MKDIR_FAILED;
                DEBUG_PRINT("status: %d, current_dir: %s", status, current_dir);
                goto lbl_cleanup;
            }
        }

        ++i;
        current_dir_len = strlen(current_dir);
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status UTILS_free(void *pointer)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    /* Free the pointer */
    HEAPFREE(pointer);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

    return status;

}


enum zash_status
UTILS_iter_dir(const char *dir_path, UTILS_iter_dir_callback_t callback, void *args)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    DIR *dir = NULL;
    struct dirent entry = {0};
    struct dirent *result = NULL;
    bool is_current_dir = false;
    bool is_parent_dir = false;
    int readdir_r_return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == dir_path) || (NULL == callback)) {
        status = ZASH_STATUS_UTILS_ITER_DIR_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the directory to iterate */
    dir = opendir(dir_path);
    if (NULL == dir) {
        status = ZASH_STATUS_UTILS_ITER_DIR_OPENDIR_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Read the first entry of the directory */
    readdir_r_return_value = readdir_r(dir, &entry, &result);
    if (C_STANDARD_SUCCESS_VALUE != readdir_r_return_value) {
        status = ZASH_STATUS_UTILS_ITER_DIR_READDIR_R_FIRST_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }


    /* While there are entries in the directory, run the callback on it. */
    while (NULL != result) {

        /* Dont run the callback on the current dir or on parent dir. */
        is_current_dir = IS_CURRENT_DIR(entry.d_name);
        is_parent_dir = IS_PARENT_DIR(entry.d_name);
        if ((false == is_current_dir) && (false == is_parent_dir)) {

            /* Run the callback on the file */
            status = callback(dir_path, entry.d_name, args);
            if (ZASH_STATUS_SUCCESS != status) {
                DEBUG_PRINT("status: %d", status);
                goto lbl_cleanup;
            }

        }

        /* Read the next entry */
        readdir_r_return_value = readdir_r(dir, &entry, &result);
        if (C_STANDARD_SUCCESS_VALUE != readdir_r_return_value) {
            status = ZASH_STATUS_UTILS_ITER_DIR_READDIR_R_FAILED;
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    CLOSEDIR(dir);

    return status;
}


enum zash_status UTILS_copy_file(const char *path, const char *new_path)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    void *source = MAP_FAILED;
    void *destination = MAP_FAILED;
    struct stat stat_buffer = {0};
    int file = INVALID_FILE_DESCRIPTOR;
    int new_file = INVALID_FILE_DESCRIPTOR;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == path) || (NULL == new_path)) {
        status = ZASH_STATUS_UTILS_COPY_FILE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the file to copy */
    file = open(path, O_RDONLY);
    if (INVALID_FILE_DESCRIPTOR == file) {
        status = ZASH_STATUS_UTILS_COPY_FILE_OPEN_FILE_FAILED;
        DEBUG_PRINT("status: %d, path: %s", status, path);
        goto lbl_cleanup;
    }

    /* Use fstat to get the file size */
    return_value = fstat(file, &stat_buffer);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_UTILS_COPY_FILE_FSTAT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Open the file to copy into */
    new_file = open(new_path, O_RDWR | O_CREAT | O_TRUNC, UTILS_NEW_FILE_PERM);
    if (INVALID_FILE_DESCRIPTOR == new_file) {
        status = ZASH_STATUS_UTILS_COPY_FILE_OPEN_NEW_FAILED;
        DEBUG_PRINT("status: %d, path: %s", status, new_path);
        goto lbl_cleanup;
    }

    /* Make sure that the new file has enough space */
    return_value = ftruncate(new_file, stat_buffer.st_size);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_UTILS_COPY_FILE_FTRUNCATE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Map into memory the file to copy */
    source = mmap(NULL, stat_buffer.st_size, PROT_READ, MAP_PRIVATE, file, 0);
    if (MAP_FAILED == source) {
        status = ZASH_STATUS_UTILS_COPY_FILE_MMAP_SOURCE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Map  into memory the file to copy into */
    destination = mmap(NULL, stat_buffer.st_size, PROT_WRITE, MAP_SHARED, new_file, 0);
    if (MAP_FAILED == destination) {
        status = ZASH_STATUS_UTILS_COPY_FILE_MMAP_DEST_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* copy the file */
    (void) memcpy(destination, source, stat_buffer.st_size);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    MUNMAP(destination, stat_buffer.st_size);
    MUNMAP(source, stat_buffer.st_size);
    CLOSE(file);
    CLOSE(new_file);

    return status;
}

enum zash_status UTILS_copy_fd(int source, int dest, size_t length)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    char *buffer = NULL;
    ssize_t return_value = C_STANDARD_FAILURE_VALUE;

    /* Allocate memory for buffer */
    buffer = HEAPALLOCZ(sizeof(*buffer) * length);
    if (NULL == buffer) {
        status = ZASH_STATUS_UTILS_COPY_FD_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Read the data from fd to copy */
    return_value = read(source, buffer, length);

    /* If no data was read, or a socket was closed, it's a different error. */
    if ((0 == return_value) ||
        ((C_STANDARD_FAILURE_VALUE == return_value) && (ECONNRESET == errno))) {
        status = ZASH_STATUS_UTILS_COPY_FD_EMPTY_FILE;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }
    if ((C_STANDARD_FAILURE_VALUE == return_value)) {
        status = ZASH_STATUS_UTILS_COPY_FD_READ_FAILED;
        perror("errork");
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Write the data read to the fd to write into. */
    return_value = write(dest, buffer, return_value);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_UTILS_COPY_FD_WRITE_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(buffer);

    return status;

}