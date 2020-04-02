/**
 * @brief This module contains error codes for the zash project.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_ZASH_STATUS_H
#define ZASH_ZASH_STATUS_H

/** Enums ************************************************/
enum zash_status {
    ZASH_STATUS_UNINITIALIZED = -1,
    ZASH_STATUS_SUCCESS = 0,

    ZASH_STATUS_BACKDOOR_MAIN_LOOP_NULL_POINTER = 1000,

    ZASH_STATUS_BACKDOOR_CREATE_NULL_POINTER,
    ZASH_STATUS_BACKDOOR_CREATE_CALLOC_FAILED,

    ZASH_STATUS_BACKDOOR_RUN_NULL_POINTER,
    ZASH_STATUS_BACKDOOR_RUN_PTHREAD_CREATE_FAILED,

    ZASH_STATUS_BACKDOOR_DESTROY_PTHREAD_JOIN_FAILED,

    ZASH_STATUS_UTILS_CGET_DIR_BY_INDEX_INVALID_INDEX = 2000,

    ZASH_STATUS_UTILS_CREATE_DIRS_NULL_POINTER,
    ZASH_STATUS_UTILS_CREATE_DIRS_STAT_FAILED,
    ZASH_STATUS_UTILS_CREATE_DIRS_FILE_EXIST,
    ZASH_STATUS_UTILS_CREATE_DIRS_MKDIR_FAILED,

    ZASH_STATUS_UTILS_ITER_DIR_NULL_POINTER,
    ZASH_STATUS_UTILS_ITER_DIR_OPENDIR_FAILED,
    ZASH_STATUS_UTILS_ITER_DIR_READDIR_R_FIRST_FAILED,
    ZASH_STATUS_UTILS_ITER_DIR_READDIR_R_FAILED,

    ZASH_STATUS_UTILS_COPY_FILE_NULL_POINTER,
    ZASH_STATUS_UTILS_COPY_FILE_OPEN_FILE_FAILED,
    ZASH_STATUS_UTILS_COPY_FILE_FSTAT_FAILED,
    ZASH_STATUS_UTILS_COPY_FILE_OPEN_NEW_FAILED,
    ZASH_STATUS_UTILS_COPY_FILE_FTRUNCATE_FAILED,
    ZASH_STATUS_UTILS_COPY_FILE_MMAP_SOURCE_FAILED,
    ZASH_STATUS_UTILS_COPY_FILE_MMAP_DEST_FAILED,

    ZASH_STATUS_VECTOR_CREATE_NULL_POINTER = 3000,
    ZASH_STATUS_VECTOR_CREATE_CALLOC_FAILED,

    ZASH_STATUS_VECTOR_PUSH_NULL_POINTER,
    ZASH_STATUS_VECTOR_PUSH_REALLOC_FAILED,

    ZASH_STATUS_VECTOR_POP_NULL_POINTER,
    ZASH_STATUS_VECTOR_POP_EMPTY_VECTOR,

    ZASH_STATUS_SCANNER_MARK_AS_VISITED_CALLOC_FAILED = 4000,

    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_SNPRINTF_FOR_SIZE_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_CALLOC_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_SNPRINTF_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FOPEN_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_FILE,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FREAD_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_LINE,

    ZASH_STATUS_SCANNER_IS_REQUIRED_STRDUP_FAILED,

    ZASH_STATUS_SCANNER_EXTRACT_DATA_CALLOC_FAILED,
    ZASH_STATUS_SCANNER_EXTRACT_DATA_GETPRIORITY_FAILED,
    ZASH_STATUS_SCANNER_EXTRACT_DATA_STRDUP_FAILED,

    ZASH_STATUS_SCANNER_SCAN_FILE_STRTOUL_FAILED,

    ZASH_STATUS_SCANNER_CREATE_NULL_POINTER,
    ZASH_STATUS_SCANNER_CREATE_CALLOC_FAILED,

    ZASH_STATUS_SCANNER_SCAN_NULL_POINTER,

    ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_NEW_FAILED = 5000,
    ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_LOG_FAILED,
    ZASH_STATUS_RUNNER_COPY_LOG_PTHREAD_RWLOCK_RDLOCK_FAILED,

    ZASH_STATUS_RUNNER_COPY_LOGS_NOT_ENOUGH_ARGUMENTS,

    ZASH_STATUS_RUNNER_CHANGE_COLOR_NOT_ENOUGH_ARGUMENTS,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_WRONG_COLOR,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_SNPRINTF_FAILED,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_WRITE_FAILED,

};

/** Macros ************************************************/
#define ZASH_UPDATE_STATUS(status, temp)    \
do {                                        \
    if (ZASH_STATUS_SUCCESS == status) {    \
        status = temp;                      \
    }                                       \
} while (0)                                 \

#endif //ZASH_ZASH_STATUS_H


