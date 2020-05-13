/**
 * @brief Internal header for daemon.c. This module make a process become a daemon process.
 * @aouther Z.F
 * @date 03/05/2020
 */

#ifndef ZASH_DAEMON_INTERNAL_H
#define ZASH_DAEMON_INTERNAL_H

/** Headers ***************************************************/
#include "daemon.h"

/** Constants ************************************************/
/* The value to pass to umask in order to remove any restrictions on file creating mode. */
#define DAEMON_UMASK_NO_RESTRICTION (0)
/* The path of the null file. */
#define DAEMON_NULL_FILE_PATH "/dev/null"

/** Functions ************************************************/
/**
 * @brief Return as a new process, and exit from the old one.
 *
 * @return      return value indicating an error may returned.
 *
 */
enum zash_status daemon_become_new_process(void);


/**
 * @brief Become a new process that is a session leader.
 *
 * @return      return value indicating an error may returned.
 *
 */
enum zash_status daemon_become_session_leader(void);


/**
 * @brief Close all opened files.
 *
 * @return     return value indicating an error may returned.
 *
 */
enum zash_status daemon_close_all_files(void);


/**
 * @brief Open a path as the standard files (stdin, stdout, stderr).
 *
 * @param [in]     path                the path of the file to become new standard files.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status daemon_open_standard_files(char *path);


#endif //ZASH_DAEMON_INTERNAL_H
