/**
 * @brief Header for utils.c. Has some common functions.
 * @aouther Z.F
 * @date 19/3/2020
 */

#ifndef ZASH_UTILS_H
#define ZASH_UTILS_H

/** Headers ***************************************************/
#include "zash_status.h"


/** Typedefs **************************************************/
/* The type of function to send to UTILS_iter_dir */
typedef enum zash_status (*UTILS_iter_dir_callback_t)(const char *dir, const char *file, void *args);


/** Functions ************************************************/
/**
 * @brief Create several directories recursively to match a path.
 *
 * @param [in]           path             the path of the directories to create.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status UTILS_create_dirs(const char *path);


/**
 * @brief Free a pointer.
 *
 * @param [in]           pointer             the pointer to free.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status UTILS_free(void *pointer);


/**
 * @brief Run a callback on each file in a directory.
 *
 * @param [in]           dir_path             the path of the directory to iterate.
 *
 * @param [in]           callback             the callack to run on each file on the directory.
 *
 * @param [in]           args                 arguments to pass the callback.
 *
 * @return               return value indicating an error may returned.
 *
 * @note                 the callback wont be ran on special files "." and ".."
 *
 */
enum zash_status
UTILS_iter_dir(const char *dir_path, UTILS_iter_dir_callback_t callback, void *args);


/**
 * @brief Copy a file into a new file.
 *
 * @param [in]           path             the path of the file to copy.
 *
 * @param [in]           new_path         the path of the new file to copy into.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status UTILS_copy_file(const char *path, const char *new_path);


/**
 * @brief Copy from one fd to another.
 *
 * @param [in]           source           the fd to copy from.
 *
 * @param [in]           dest             the fd to copy into.
 *
 * @param [in]           length           the length of data to copy, in bytes.
 *
 * @return               return value indicating an error may returned.
 *
 */
enum zash_status UTILS_copy_fd(int source, int dest, size_t length);


#endif //ZASH_UTILS_H
