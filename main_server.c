/**
 * @brief This module use the server module.
 * @aouther Z.F
 * @date 07/05/2020
 */

/** Headers ************************************************/
#include "zash_status.h"
#include "common.h"
#include "server/server.h"

/** Constants ************************************************/
/* The port that server should listen for port knocking on */
#define MAIN_SERVER_LISTENING_PORT (2020)

/** Enums ****************************************************/
/* The parameters for the main function */
enum main_server_parameter {
    MAIN_SERVER_PARAMETER_PROGRAM_NAME = 0,
    MAIN_SERVER_PARAMETER_INTERFACE_NAME,
    MAIN_SERVER_PARAMETER_MAX_PARAMETERS,
};


/** Functions ************************************************/
/**
 * @brief Run the Server.
 *
 * @return               return value indicating an error may returned.
 *
 */
int main(int argc, char *argv[])
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int exit_status = EXIT_FAILURE;

    /* Check for valid parameter count */
    if (MAIN_SERVER_PARAMETER_MAX_PARAMETERS > argc) {
        status = ZASH_MAIN_SERVER_MAIN_INVALID_PARAMETERS;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Run the server */
    status = SERVER_run(MAIN_SERVER_LISTENING_PORT, argv[MAIN_SERVER_PARAMETER_INTERFACE_NAME]);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate success*/
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (ZASH_STATUS_SUCCESS == status) {
        exit_status = EXIT_SUCCESS;
    }
    else {
        exit_status = EXIT_FAILURE;
    }

    return exit_status;

}