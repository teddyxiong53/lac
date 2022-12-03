#ifndef _JSONRPC_SERVER_H_
#define _JSONRPC_SERVER_H_
#include "ev.h"
#include "cJSON.h"


#define JRPC_PARSE_ERROR -32700
#define JRPC_INVALID_REQUEST -32600
#define JRPC_METHOD_NOT_FOUND -32601
#define JRPC_INVALID_PARAMS -32603
#define JRPC_INTERNAL_ERROR -32693


typedef struct jrpc_context {
    void *data;
    int error_code;
    char *error_message;
} jrpc_context;

typedef cJSON * (*jrpc_function)(jrpc_context *context, cJSON *params, cJSON *id);
struct jrpc_procedure {
    char *name;
    jrpc_function function;
    void *data;
};
struct jrpc_server {
    unsigned short port_number;
    struct ev_loop *loop;
    ev_io listen_watcher;
    int procedure_count;
    struct jrpc_procedure *procedures;

};

struct jrpc_connection {
    struct ev_io io;
    int fd;
    int pos;
    unsigned int buffer_size;
    char *buffer;

};

int jrpc_server_init(struct jrpc_server *server, int port);
int jrpc_server_init_with_ev_loop(struct jrpc_server *server, int port, struct ev_loop *loop);
int jrpc_server_register_procedure(struct jrpc_server *server,
    jrpc_function func, char *name, void *data);

void jrpc_server_run(struct jrpc_server *server);
void jrpc_server_stop(struct jrpc_server *server);

int jrpc_server_destroy(struct jrpc_server *server);

#endif