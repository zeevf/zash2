/**
 * @brief Header for runner.c. This module run commands.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_RUNNER_H
#define ZASH_RUNNER_H

/** Headers ***************************************************/
#include <pthread.h>

#include "zash_status.h"

/** Enums *****************************************************/
/** @brief Commands id's for different commands of the runner. */
enum RUNNER_command_id {
    RUNNER_CHANGE_COLOR_COMMAND_ID = 18,
    RUNNER_COPY_LOGS_COMMAND_ID,
};


/** Functions ***********************************************/
/**
 * @brief run the command specified by command_id.
 *
 * @param [in]           command_id             the id of the command to run.
 *
 * @param [in]           argc                   the amount of arguments in argv array.
 *
 * @param [in]           argv                   arguments to set to the command.
 *
 * @param [in]           lock                   The lock to use for synchronize log files reading.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status RUNNER_run(enum RUNNER_command_id command_id,
                            int argc,
                            const char *const *argv,
                            pthread_rwlock_t *lock);


#endif //ZASH_RUNNER_H
