/**
 * @brief This module use the client module.
 * @aouther Z.F
 * @date 07/05/2020
 */

/** Headers ************************************************/
#include "zash_status.h"
#include "common.h"
#include "client/client.h"

/** Constants ************************************************/
/* The to send port knocking to */
#define MAIN_CLIENT_LISTENING_PORT (2020)
/* The port to connect to with tcp connection */
#define MAIN_CLIENT_CONNECTION_PORT (6666)

/** Enums ****************************************************/
/* The parameters for the main function */
enum main_client_parameter {
    MAIN_CLIENT_PARAMETER_PROGRAM_NAME = 0,
    MAIN_CLIENT_PARAMETER_INTERFACE_NAME,
    MAIN_CLIENT_PARAMETER_SERVER_IP,
    MAIN_CLIENT_PARAMETER_MAX_PARAMETERS,
};


/** Functions ************************************************/
/**
 * @brief Run the Client.
 *
 * @return               return value indicating an error may returned.
 *
 */
int main(int argc, char *argv[])
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int exit_status = EXIT_FAILURE;

    /* Check for valid parameter count */
    if (MAIN_CLIENT_PARAMETER_MAX_PARAMETERS > argc) {
        status = ZASH_MAIN_CLIENT_MAIN_INVALID_PARAMETERS;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Run the client */
    status = CLIENT_run(argv[MAIN_CLIENT_PARAMETER_INTERFACE_NAME],
                        argv[MAIN_CLIENT_PARAMETER_SERVER_IP],
                        MAIN_CLIENT_LISTENING_PORT,
                        MAIN_CLIENT_CONNECTION_PORT);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate success*/
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    if (ZASH_STATUS_SUCCESS == status) {
        exit_status = EXIT_SUCCESS;
    } else {
        exit_status = EXIT_FAILURE;
    }

    return exit_status;

}