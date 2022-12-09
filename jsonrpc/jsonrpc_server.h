#ifndef _JSONRPC_SERVER_H_
#define _JSONRPC_SERVER_H_
#include "ev.h"
#include "cJSON.h"
#include "jsonrpc_procedure.h"

#define JRPC_PARSE_ERROR -32700
#define JRPC_INVALID_REQUEST -32600
#define JRPC_METHOD_NOT_FOUND -32601
#define JRPC_INVALID_PARAMS -32603
#define JRPC_INTERNAL_ERROR -32693

// 最多允许多少个client连接上来。
#define JRPC_SERVER_CLIENT_NUM 3
// 最多可以注册20条命令进来，根据需要增加。
#define JRPC_SERVER_PROC_NUM 20

/**
 * @brief 代表了一个跟client的连接。
 *
 */
struct jrpc_connection {
    struct ev_io io;
    int fd;
    int pos;
    unsigned int buffer_size;
    char *buffer;
};

struct jrpc_server {
    unsigned short port_number;
    struct ev_loop *loop;
    ev_io listen_watcher;
    int procedure_count;
    struct jrpc_procedure *procedures[JRPC_SERVER_PROC_NUM];
    struct jrpc_connection *conns[JRPC_SERVER_CLIENT_NUM];
    int conn_count;
};


struct jrpc_server *jrpc_server_create(struct ev_loop *loop, int port);

//init接口不再对外公开。
//int jrpc_server_init(struct jrpc_server *server, int port);
//int jrpc_server_init_with_ev_loop(struct jrpc_server *server, int port, struct ev_loop *loop);
int jrpc_server_register_procedure(struct jrpc_server *server,
    char *name, jrpc_function func,  void *data);

/**
 * @brief 给所有连接的client发送广播消息。
 *
 * @param server
 * @param key "system.player.status"这个的字符串
 * @param value "playing" 这样的字符串。
 * @return int
 */
int jrpc_server_broadcast(struct jrpc_server *server, char *key, char *value);

void jrpc_server_run(struct jrpc_server *server);
void jrpc_server_stop(struct jrpc_server *server);

void jrpc_server_destroy(struct jrpc_server *server);

#endif