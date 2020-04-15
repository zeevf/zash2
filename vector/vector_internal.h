/**
 * @brief Internal header for vector.c
 *        handle a vector object, a data structure that enabling pushing and popping.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_VECTOR_INTERNAL_H
#define ZASH_VECTOR_INTERNAL_H

/** Headers ***************************************************/
#include "vector.h"


/** Constants ************************************************/
/* The size allocated each time for the vector. */
#define VECTOR_ALLOCATING_BLOCK_SIZE (256)


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


#endif //ZASH_VECTOR_INTERNAL_H
