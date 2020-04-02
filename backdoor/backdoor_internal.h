/**
 * @brief Internal header for backdoor.c. The backdoor reading commands using execute them.
 * @aouther Z.F
 * @date 22/3/2020
 */

#ifndef ZASH_BACKDOOR_INTERNAL_H
#define ZASH_BACKDOOR_INTERNAL_H

/** Headers ***************************************************/
#include <stdbool.h>
#include <pthread.h>

#include "backdoor.h"
/** Structs ***************************************************/
/** @brief the context of a backdoor context. */
struct BACKDOOR_context {
    /* the thread id of the new thread running the backdoor. */
    pthread_t thread_id;
    /* true if the new thread is running. */
    bool is_thread_running;
    /* true if the thread running the backdoor should stop. */
    bool should_stop;
};


/** Typedefs *************************************************/
/* the function type to pass to pthread_create. */
typedef void *(*backdoor_pthread_func_t)(void *);


/** Functions ***********************************************/
/**
 * @brief Run each command in an array.
 *
 * @param [in]           commands                   commands information to run.
 *
 * @param [in]           commands_amount            the amount of commands to run.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 the function dont return an error if a command have failed, so the backdoor
 *                       will continue to run.
 */
enum zash_status
backdoor_run_commands(const struct SCANNER_data *const commands[], size_t commands_amount);


/**
 * @brief Scan for commands and execute them.
 *
 * @param [in]           should_stop           true when should stop listening for commands.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 this function will not return until should_stop would change to true.
 *
 */
enum zash_status backdoor_main_loop(const bool *should_stop);


#endif //ZASH_BACKDOOR_INTERNAL_H
