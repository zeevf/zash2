/**
 * @brief Header for runner.c. This module run commands.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_RUNNER_H
#define ZASH_RUNNER_H

/** Headers ***************************************************/
#include "zash_status.h"

/** Constants *************************************************/
/* The code of the command for copy the logs file into new directory. */
#define RUNNER_COPY_LOGS_ID (19)
/* The code of the command for changing the terminal color. */
#define RUNNER_CHANGE_COLOR_ID (18)


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
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status RUNNER_run(int command_id, int argc, const char *const argv[]);


#endif //ZASH_RUNNER_H
