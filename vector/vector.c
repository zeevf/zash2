/**
 * @brief This module handle a vector object, a data structure that enabling pushing and popping.
 * @aouther Z.F
 * @date 19/3/2020
 */

/** Headers ***************************************************/
#include "common.h"

#include "vector.h"
#include "vector_internal.h"


/** Functions ************************************************/
enum zash_status VECTOR_create(struct VECTOR_context **context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct VECTOR_context *temp_context = NULL;

    /* Check for valid parameters */
    if (NULL == context) {
        status = ZASH_STATUS_VECTOR_CREATE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for new vector */
    temp_context = HEAPALLOCZ(sizeof(*temp_context));
    if (NULL == temp_context) {
        status = ZASH_STATUS_VECTOR_CREATE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Initialize Vector fields */
    temp_context->array = NULL;
    temp_context->array_max = 0;
    temp_context->array_size = 0;

    /* Transfer Ownership */
    *context = temp_context;
    temp_context = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(temp_context);

    return status;

}


enum zash_status VECTOR_push(struct VECTOR_context *context, void *entry)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    void **realloc_return_value = NULL;

    /* Check for valid parameters */
    if (NULL == context) {
        status = ZASH_STATUS_VECTOR_PUSH_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* If there is not enough space in the vector array, realloc it to add more memory. */
    if (context->array_size >= context->array_max) {

        /* Increase maximum array size */
        context->array_max += VECTOR_ALLOCATING_BLOCK_SIZE;

        /* Realloc the memory for the vector */
        realloc_return_value = realloc(context->array,
                                       context->array_max * sizeof(*realloc_return_value));
        if (NULL == realloc_return_value) {
            status = ZASH_STATUS_VECTOR_PUSH_REALLOC_FAILED;
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
        context->array = realloc_return_value;
    }

    /* Add the new pointer to the vector */
    context->array[context->array_size] = entry;
    context->array_size++;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;

}


enum zash_status VECTOR_pop(struct VECTOR_context *context, void **entry)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    void *temp_entry = NULL;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == entry)) {
        status = ZASH_STATUS_VECTOR_POP_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Check if the Vector contains any data to pop */
    if (0 >= context->array_size) {
        status = ZASH_STATUS_VECTOR_POP_EMPTY_VECTOR;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the latest object form the vector and remove it */
    context->array_size--;
    temp_entry = context->array[context->array_size];
    context->array[context->array_size] = NULL;

    /* Transfer Ownership */
    *entry = temp_entry;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status
VECTOR_as_array(struct VECTOR_context *context, void const *const **array, size_t *array_length)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    void *temp_entry = NULL;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == array) || (NULL == array_length)) {
        status = ZASH_STATUS_VECTOR_AS_ARRAY_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *array = (void const *const *) context->array;
    *array_length = context->array_size;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status VECTOR_destroy(struct VECTOR_context *context, VECTOR_free_func_t free_func)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    size_t i = 0;

    /* Destroy all data in the vector, if a free function was supplied. */
    if ((NULL != context) && (NULL != free_func)) {

        /* For each object in the vector, free it. */
        for (i = 0; i < context->array_size; ++i) {

            /* Free the current object */
            temp_status = free_func(context->array[i]);
            if (ZASH_STATUS_SUCCESS != temp_status) {
                DEBUG_PRINT("status: %d", status);
                ZASH_UPDATE_STATUS(status, temp_status);
            }
        }
    }

    if (NULL != context) {
        HEAPFREE(context->array);
    }
    HEAPFREE(context);

    /* If no error status was set, indicate success */
    ZASH_UPDATE_STATUS(status, ZASH_STATUS_SUCCESS);

    return status;
}