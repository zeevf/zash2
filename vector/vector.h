/**
 * @brief Internal header for vector.c
 *        handle a vector object, a data structure that enabling pushing and popping.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_VECTOR_H
#define ZASH_VECTOR_H

/** Headers ***************************************************/
#include <stdlib.h>

#include "zash_status.h"


/** Structs ***************************************************/
/** @brief The context of vector object */
struct VECTOR_context {
    /* An array of pointer, contains the vector content. */
    void **array;
    /* The amount of objects in the array. */
    size_t array_size;
    /* The amount of objects that the array can contain. */
    size_t array_max;
};


/** Typedefs *************************************************/
/* The type of function used to free objects in the vector. */
typedef enum zash_status (*VECTOR_free_func_t)(void *entry);

/** Functions ************************************************/
/**
 * @brief Create a Vector object.
 *
 * @param [out]          context                the newly created vector object.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 must be freed using VECTOR_destroy.
 *
 */
enum zash_status VECTOR_create(struct VECTOR_context **context);


/**
 * @brief Push a pointer into the vector
 *
 * @param [out]          context                the Vector to push the pointer into
 *
 * @param [out]          entry                  the pointer to push.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status VECTOR_push(struct VECTOR_context *context, void *entry);


/**
 * @brief Remove the latest pointer added to a vector and get it.
 *
 * @param [out]          context                the Vector to pop from
 *
 * @param [out]          entry                  the pointer popped.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status VECTOR_pop(struct VECTOR_context *context, void **entry);


/**
 * @brief Free a Vector object, and all of the object it contains.
 *
 * @param [in]           context          the vector to free
 *
 * @param [in, opt]      free_func        the Function to use for freeing the objects in the vector.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status VECTOR_destroy(struct VECTOR_context *context, VECTOR_free_func_t free_func);


#endif //ZASH_VECTOR_H
