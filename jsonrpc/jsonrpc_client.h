#ifndef _JSONRPC_CLIENT_H_
#define _JSONRPC_CLIENT_H_

#include "cJSON.h"
#include "ev.h"

#define JSONRPC_DEFAULT_TIMEOUT 500 //500ms
#define JSONRPC_DEFAULT_BUFFER_SIZE 8192

#define JRPC_CLIENT_BUF_LEN 1024

struct jrpc_client {
    int sockfd;//连接server的socket fd
    char *server_ip;//要连接的服务器的地址
    unsigned short port;//要连接的jsonrpc服务器的端口

    struct ev_loop *loop;
    ev_io watcher;

    char *send_buf;
    int send_buf_size;
    char *recv_buf;
    int recv_buf_size;

    int cur_msg_id;//存放当前自己发送的jsonrpc消息的id值。
                    //用来判断收到的内容是不是自己发送的消息的返回值。

};

int jrpc_client_init(struct jrpc_client* client, char* host, int port);
int jrpc_client_connect_server(struct jrpc_client *client);
void jrpc_client_run(struct jrpc_client *client);
int jrpc_client_send_cmd(struct jrpc_client *client, cJSON *cmd);
int jrpc_client_send_cmd_only_name(struct jrpc_client *client, char *cmd);

#endif
