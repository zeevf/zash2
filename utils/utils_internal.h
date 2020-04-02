/**
 * @brief Internal header for utils.c. Has some common functions.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_UTILS_INTERNAL_H
#define ZASH_UTILS_INTERNAL_H

/** Headers ***************************************************/
#include <stdlib.h>


/** Constants ************************************************/
/* The mode to create new file in */
#define UTILS_NEW_FILE_PERM (0664)
/* The mode to create new directory in */
#define UTILS_NEW_DIR_PERM (0775)
/* The sign represent a directory */
#define UTILS_DIRECTORY_SIGN ('/')

/** Functions ************************************************/
/**
 * @brief Get the path of a directory from a path of directories.
 *
 * @param [in]           path             the path of directories to get a directory path from.
 *
 * @param [in]           index            the index of directory to get it's found.
 *
 * @param [in]           dir_max          the maximum length can be write into dir parameter.
 *
 * @param [out]          dir              the path of the directory specified by index.
 *
 * @return               return value indicating an error may returned.
 */
enum zash_status utils_get_dir_by_index(const char *path, size_t index, size_t dir_max, char *dir);


#endif //ZASH_UTILS_INTERNAL_H
