//
// Created by user on 5/4/20.
//

#include "common.h"
#include "shell.h"


struct SHELL_context {
    char something;
};


enum zash_status SHELL_create(struct SHELL_context **context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    *context = HEAPALLOCZ(sizeof(**context));

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status SHELL_run(struct SHELL_context *context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    UNREFERENCED_PARAMETER(context);

    system("bash");

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status SHELL_destroy(struct SHELL_context *context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    HEAPFREE(context);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}
