//
// Created by user on 3/19/20.
//

#ifndef ZASH_COMMON_H
#define ZASH_COMMON_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>


#ifdef _DEBUG
#define DEBUG_PRINT(...) do {fprintf(stderr, "DEBUG:%s:%s:%d:  ",__FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");} while (0)
#else
#define DEBUG_PRINT(format, args...)
#endif

#ifdef _DEBUG
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(args...)
#endif


#ifdef _DEBUG
#define STATIC
#else
#define STATIC static
#endif


#define HEAPALLOCZ(size) calloc(1, size)
#define ZERO_MEMORY(pointer, size) (void)memset(pointer, 0, size)
#define UNREFERENCED_PARAMETER(param) ((param) = (param))
#define IS_CURRENT_DIR(file) (0 == strncmp(file, ".", sizeof(".")));
#define IS_PARENT_DIR(file) (0 == strncmp(file, "..", sizeof(".")));
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define HEAPFREE(pointer) do {if (NULL != pointer){(void)free(pointer); pointer = NULL;}} while (0)
#define CLOSEDIR(dir) do {if (NULL != dir){(void)closedir(dir); dir = NULL;}} while (0)
#define CLOSE(fd) do {if (-1 != fd){(void)close(fd); fd = -1;}} while (0)
#define FCLOSE(file) do {if (NULL != file){(void)fclose(file); file = NULL;}} while (0)
#define MUNMAP(pointer, size) do {if (pointer != MAP_FAILED){(void)munmap(pointer, size); pointer = MAP_FAILED;}} while (0)

#define C_STANDARD_FAILURE_VALUE (-1)
#define C_STANDARD_SUCCESS_VALUE (0)
#define ERRNO_SUCCESS (0)
#ifndef PATH_MAX
#define PATH_MAX (8192)
#endif
#define NULL_TERMINATOR ('\0')
#define INVALID_PROCESS_ID (-1)
#define CHILD_PROCESS (0)
#define INVALID_FILE_DESCRIPTOR (-1)
#define FOPEN_READ_ONLY "r"
#define DECIMAL_BASE (10)
#define ROOT_DIRECTORY "/"

#endif //ZASH_COMMON_H
