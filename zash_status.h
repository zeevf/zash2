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

    ZASH_STATUS_VECTOR_AS_ARRAY_NULL_POINTER,
    ZASH_STATUS_VECTOR_AS_ARRAY_CALLOC_FAILED,

    ZASH_STATUS_SCANNER_MARK_AS_VISITED_CALLOC_FAILED = 4000,

    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_SNPRINTF_FOR_SIZE_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_CALLOC_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_SNPRINTF_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FOPEN_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_FILE,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_FREAD_FAILED,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_LINE,
    ZASH_STATUS_SCANNER_GET_COMMAND_LINE_COMMAND_LINE_TOO_LONG,

    ZASH_STATUS_SCANNER_IS_REQUIRED_STRDUP_FAILED,

    ZASH_STATUS_SCANNER_GET_COMMAND_ID_NO_PROCESS_EXIST,
    ZASH_STATUS_SCANNER_GET_COMMAND_ID_GETPRIORITY_FAILED,

    ZASH_STATUS_SCANNER_DATA_CREATE_STRDUP_FAILED,
    ZASH_STATUS_SCANNER_DATA_CREATE_CALLOC_FAILED,

    ZASH_STATUS_SCANNER_SCAN_FILE_STRTOUL_FAILED,

    ZASH_STATUS_SCANNER_CREATE_NULL_POINTER,
    ZASH_STATUS_SCANNER_CREATE_CALLOC_FAILED,

    ZASH_STATUS_SCANNER_SCAN_NULL_POINTER,

    ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_NEW_FAILED = 5000,
    ZASH_STATUS_RUNNER_COPY_LOG_NEW_PATH_TOO_LONG,
    ZASH_STATUS_RUNNER_COPY_LOG_SNPRINTF_LOG_FAILED,
    ZASH_STATUS_RUNNER_COPY_LOG_LOG_PATH_TOO_LONG,

    ZASH_STATUS_RUNNER_COPY_LOGS_NOT_ENOUGH_ARGUMENTS,
    ZASH_STATUS_RUNNER_COPY_LOGS_PTHREAD_RWLOCK_RDLOCK_FAILED,
    ZASH_STATUS_RUNNER_COPY_LOGS_PTHREAD_RWLOCK_UNLOCK_FAILED,

    ZASH_STATUS_RUNNER_CHANGE_COLOR_NOT_ENOUGH_ARGUMENTS,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_WRONG_COLOR,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_SNPRINTF_FAILED,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_COMMAND_TRUNCATED,
    ZASH_STATUS_RUNNER_CHANGE_COLOR_WRITE_FAILED,

    ZASH_STATUS_RUNNER_RUN_NULL_POINTER,
    ZASH_STATUS_RUNNER_RUN_NO_COMMANDS,

    ZASH_STATUS_DAEMON_BECOME_SESSION_LEADER_FORK_FAILED = 6000,
    ZASH_STATUS_DAEMON_BECOME_SESSION_LEADER_SETSID_FAILED,

    ZASH_STATUS_DAEMON_CLOSE_ALL_FILES_SYSCONF_FAILED,

    ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_OPEN_FAILED,
    ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_DUP_STDIN_FAILED,
    ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_DUP_STDOUT_FAILED,
    ZASH_STATUS_DAEMON_OPEN_STANDARD_FILES_DUP_STDERR_FAILED,

    ZASH_STATUS_DAEMON_DAEMONIZE_CHDIR_FAILED,

    ZASH_STATUS_SOCKET_BIND_INTERFACE_SETSOCKOPT_FAILED = 7000,

    ZASH_STATUS_SOCKET_GET_TCP_CHECKSUM_CALLOC_FAILED,

    ZASH_STATUS_SOCKET_GET_ADDRESS_INET_PTON_FAILED,

    ZASH_STATUS_SOCKET_GET_SYN_FILTER_CALLOC_FAILED,

    ZASH_STATUS_SOCKET_ATTACH_SYN_FILTER_SETSOCKOPT_FAILED,

    ZASH_STATUS_SOCKET_TCP_CONNECT_CONNECT_FAILED,

    ZASH_STATUS_SOCKET_SYN_CREATE_NULL_POINTER,
    ZASH_STATUS_SOCKET_SYN_CREATE_CALLOC_FAILED,
    ZASH_STATUS_SOCKET_SYN_CREATE_SOCKET_FAILED,
    ZASH_STATUS_SOCKET_SYN_CREATE_IOCTL_FAILED,

    ZASH_STATUS_SOCKET_SYN_SEND_NULL_POINTER,
    ZASH_STATUS_SOCKET_SYN_SEND_CALLOC_FAILED,
    ZASH_STATUS_SOCKET_SYN_SEND_SEND_FAILED,

    ZASH_STATUS_SOCKET_SYN_RECEIVE_NULL_POINTER,
    ZASH_STATUS_SOCKET_SYN_RECEIVE_CALLOC_FAILED,
    ZASH_STATUS_SOCKET_SYN_RECEIVE_RECV_FAILED,

    ZASH_STATUS_SOCKET_TCP_SERVER_NULL_POINTER,
    ZASH_STATUS_SOCKET_TCP_SERVER_SOCKET_FAILED,
    ZASH_STATUS_SOCKET_TCP_SERVER_BIND_FAILED,
    ZASH_STATUS_SOCKET_TCP_SERVER_LISTEN_FAILED,
    ZASH_STATUS_SOCKET_TCP_SERVER_ACCEPT_FAILED,

    ZASH_STATUS_SOCKET_TCP_CLIENT_NULL_POINTER,
    ZASH_STATUS_SOCKET_TCP_CLIENT_SOCKET_FAILED,

    ZASH_STATUS_CLIENT_COPY_FD_CALLOC_FAILED = 8000,
    ZASH_STATUS_CLIENT_COPY_FD_READ_FAILED,
    ZASH_STATUS_CLIENT_COPY_FD_EMPTY_FILE,
    ZASH_STATUS_CLIENT_COPY_FD_WRITE_FAILED,

    ZASH_STATUS_CLIENT_CONNECT_TERMINAL_TO_SOCKET_SELECT_FAILED,

    ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDIN_FAILED = 9000,
    ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDOUT_FAILED,
    ZASH_STATUS_SERVER_RUN_SHELL_DUP2_STDERR_FAILED,

    ZASH_STATUS_SERVER_RUN_NULL_POINTER,

    ZASH_STATUS_MAIN_PTHREAD_RWLOCK_DESTROY_FAILED = 10000,
};

/** Macros ************************************************/
#define ZASH_UPDATE_STATUS(status, temp)                                                \
do {                                                                                    \
    if ((ZASH_STATUS_SUCCESS == status) || (ZASH_STATUS_UNINITIALIZED == status)) {     \
        status = temp;                                                                  \
    }                                                                                   \
} while (0)                                                                             \

#endif //ZASH_ZASH_STATUS_H


