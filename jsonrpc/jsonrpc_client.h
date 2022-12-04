#ifndef _JSONRPC_CLIENT_H_
#define _JSONRPC_CLIENT_H_

#include "cJSON.h"
#include "ev.h"
#include <pthread.h>

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
/**
 * @brief 发送rpc请求。
 *
 * @param client
 * @param cmd 如果里面带有id，那么就是request，没有带id，那就是notification。
 *              二者的区别的：request要求返回，notification不要求返回结果。
 * @return int
 */
int jrpc_client_send_cmd(struct jrpc_client *client, cJSON *cmd);
/**
 * @brief 参数为字符串的发送函数。
 *
 * @param client
 * @param cmd 可以是一个简单的字符串，也可以是一个json字符串。
 *              如果是简单字符串，当前一个不带param的notification进行发送。
 * @return int
 */
int jrpc_client_send_cmd_str(struct jrpc_client *client, char *cmd);

/**
 * @brief 返回一个唯一的id值。调用者用这个来构造一个唯一的id。
 *
 * @return int id值。
 */
int jrpc_client_get_unique_id();

#endif
