#ifndef _JSONRPC_PROCEDURE_H_
#define _JSONRPC_PROCEDURE_H_

/**
 * @file jsonrpc_procedure.h
 * @author your name (you@domain.com)
 * @brief 这个文件是存放jsonrpc处理命令的结构体的相关接口。可以被jsonrpc server和jsonrpc client使用。
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "cJSON.h"

struct jrpc_context {
    void *data;
    int error_code;
    char *error_message;
} ;

typedef cJSON * (*jrpc_function)(struct jrpc_context *context, cJSON *params, cJSON *id);
struct jrpc_procedure {
    char *name;
    jrpc_function function;
    void *data;
};

struct jrpc_procedure * jrpc_procedure_create(char *name, jrpc_function func, void *data);

void jrpc_procedure_destroy(struct jrpc_procedure * proc);


#endif
