/**
 * @brief Header for scanner.c. This module scan for commands.
 * @aouther Z.F
 * @date 22/3/2020
 */

#ifndef ZASH_SCANNER_H
#define ZASH_SCANNER_H

/** Headers ***************************************************/
#include <stdlib.h>

#include "vector/vector.h"

#include "zash_status.h"

/** Structs **************************************************/
/** @brief the data about command that scanned */
struct SCANNER_data {
    /* the id of the command */
    int id;
    /* the arguments for the command */
    struct VECTOR_context *argv;
};

/** @brief the context of a scanner object. */
struct SCANNER_context;


/** Functions *************************************************/
/**
 * @brief Free a SCANNER_data object.
 *
 * @param [in]           data                 the data to free
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status SCANNER_free_data(struct SCANNER_data *data);


/**
 * @brief Create a Scanner context.
 *
 * @param [out]          context                the newly created scanner object.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 must be freed using SCANNER_destroy.
 *
 */
enum zash_status SCANNER_create(struct SCANNER_context **context);


/**
 * @brief Scan for new commands. retrieve all the data found about the commands to run.
 *
 * @param [in]           context                the scanner object to scan with
 *
 * @param [out]          data                   the data found about the commands.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 once a command scanned successfully using this scanner, it cant be scanned
 *                       again with the same scanner.
 *
 */
enum zash_status SCANNER_scan(struct SCANNER_context *context, struct VECTOR_context **data);


/**
 * @brief Free a scanner object.
 *
 * @param [in]           context               the scanner to free
 *
 * @return               return value indicating an error may returned.
 *
 */

enum zash_status SCANNER_destroy(struct SCANNER_context *context);


#endif //ZASH_SCANNER_H
