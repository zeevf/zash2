/**
 * @brief Internal header for runner.c. This module run commands.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_RUNNER_INTERNAL_H
#define ZASH_RUNNER_INTERNAL_H

/** Headers ***************************************************/
#include "runner.h"


/** Constants *************************************************/
/* The format to use for concat directory paths */
#define RUNNER_DIR_FORMAT "%s/%s"
/* The code to sent to the terminal for changing color */
#define RUNNER_COLOR_COMMAND "\x1b[0;3%cm"
/* The length in bytes of new color command */
#define RUNNER_COLOR_COMMAND_LEN (8)
/* The minimum valid value to pass as color. */
#define RUNNER_MIN_VALID_COLOR ('0')
/* The maximum valid value to pass as color. */
#define RUNNER_MAX_VALID_COLOR ('7')


/** Macros ****************************************************/
/* Check if a character can represent a color. */
#define RUNNER_IS_VALID_COLOR(color) ((RUNNER_MIN_VALID_COLOR > (color)) || \
                                      (RUNNER_MAX_VALID_COLOR < (color)))   \


/** Enums *****************************************************/
/** @brief the parameters of copy logs commands */
enum runner_copy_logs_parameter {
    /* the path of the directory to copy logs into */
    RUNNER_COPY_LOGS_PARAMETER_DESTINATION_DIR = 0,
    /* the maximum amount of parameters that this commands receives */
    RUNNER_COPY_LOGS_PARAMETER_MAX_PARAMETERS,
};

/** @brief the parameters of copy logs commands */
enum runner_change_color_parameter {
    /* the color to change the terminal output into */
    RUNNER_CHANGE_COLOR_PARAMETER_NEW_COLOR = 0,
    /* the maximum amount of parameters that this commands receives */
    RUNNER_CHANGE_COLOR_PARAMETER_MAX_PARAMETERS,
};


/** Functions *************************************************/
/**
 * @brief copy a log from the log directory into a new directory.
 *
 * @param [in]           log_dir               the path of the directory where the logs are.
 *
 * @param [in]           log_file              the name of the log file to copy.
 *
 * @param [in]           dir                   the path of the directory to copy the log file into.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 This function is a callback for UTILS_iter_dir
 *
 */
enum zash_status runner_copy_log(const char *log_dir, const char *log_file, const char *dir);


/**
 * @brief copy the log files from the log directory into a new directory.
 *
 * @param [in]           argc                  the amount of arguments to in argv
 *
 * @param [in]           argv                  the arguments for the command. should contain
 *                                             the path of the new directory.
 *
 * @param [in]           lock                  The lock to use for synchronize log files reading.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status runner_copy_logs(int argc, const char *const argv[], pthread_rwlock_t *lock);


/**
 * @brief change the color of the terminal.
 *
 * @param [in]           argc                  the amount of arguments to in argv
 *
 * @param [in]           argv                  the arguments for the command. should contain
 *                                             the new color of the terminal.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status runner_change_color(int argc, const char *const argv[]);


#endif //ZASH_RUNNER_INTERNAL_H
