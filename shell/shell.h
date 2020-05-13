//
// Created by user on 5/4/20.
//

#ifndef ZASH_SHELL_H
#define ZASH_SHELL_H

#include "zash_status.h"

struct SHELL_context;

enum zash_status SHELL_create(struct SHELL_context **context);
enum zash_status SHELL_run(struct SHELL_context *context);
enum zash_status SHELL_destroy(struct SHELL_context *context);

#endif //ZASH_SHELL_H
