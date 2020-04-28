/**
 * @brief Header for daemon.c. This module make a process become a daemon process.
 * @aouther Z.F
 * @date 03/05/2020
 */


#ifndef ZASH_DEAMON_H
#define ZASH_DEAMON_H

/** Headers ***************************************************/
#include "zash_status.h"

/** Functions ************************************************/

/**
 * @brief Make this process a daemon process. that means:
 *          -  the process will become a new process that is a session leader, with init as
 *             parent process.
 *          -  the umask will be reset.
 *          -  the cwd will become root directory.
 *          -  all opened file will be closed, and the standard file would be "/dev/null".
 *
 * @return     return value indicating an error may returned.
 *
 */
enum zash_status DAEMON_daemonize(void);


#endif //ZASH_DEAMON_H
