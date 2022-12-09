#include "jsonrpc_server.h"
#include "mylog.h"
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "backtrace_print.h"

struct ev_loop *loop;

static void close_connection(struct ev_loop *loop, ev_io *w)
{
    mylogd("close conection");
    ev_io_stop(loop ,w);
    struct jrpc_connection *conn = (struct jrpc_connection *)w;
    struct jrpc_server *server = w->data;
    // 从server里找到这个conn。把对应的位置清空。
    int i;
    for (i=0; i<JRPC_SERVER_CLIENT_NUM; i++) {
        if (conn == server->conns[i]) {
            server->conns[i] = NULL;
            server->conn_count --;
        }
    }
    close(conn->fd);
    free(conn->buffer);
    free(conn);
}

static int send_response(struct jrpc_connection *conn, char *response)
{
    int fd = conn->fd;
    write(fd, response, strlen(response));
    write(fd, "\n", 1);
    return 0;
}

static int send_error(struct jrpc_connection *conn, int code, char *message, cJSON *id)
{
    int ret = 0;
    cJSON *result_root = cJSON_CreateObject();
    cJSON *error_root = cJSON_CreateObject();
    cJSON_AddNumberToObject(error_root, "code", code);
    cJSON_AddStringToObject(error_root, "message", message);
    cJSON_AddItemToObject(result_root, "error", error_root);
    cJSON_AddItemToObject(result_root, "id", id);
    char *str_result = cJSON_Print(result_root);
    ret = send_response(conn, str_result);
    free(str_result);
    cJSON_Delete(result_root);
    free(message);
    return ret;
}

static int send_result(struct jrpc_connection *conn, cJSON *result, cJSON *id)
{
    int ret = 0;
    cJSON *result_root = cJSON_CreateObject();
    if (result) {
        cJSON_AddItemToObject(result_root, "result", result);
    }
    cJSON_AddItemToObject(result_root, "id", id);
    char *str = cJSON_Print(result_root);
    ret = send_response(conn, str);
    free(str);
    cJSON_Delete(result_root);
    return ret;
}


static int invoke_procedure(struct jrpc_server *server, struct jrpc_connection *conn,
    char *name, cJSON *params, cJSON *id)
{
    cJSON *ret = NULL;
    int found = 0;
    struct jrpc_context ctx;
    ctx.error_code = 0;
    ctx.error_message = NULL;
    int i;
    for (i=0; i<server->procedure_count; i++) {
        mylogd("%s == %s", server->procedures[i]->name, name);
        if (strcmp(server->procedures[i]->name, name) == 0) {
            found = 1;
            ctx.data = server->procedures[i]->data;
            mylogd("");
            ret = server->procedures[i]->function(&ctx, params, id);
            mylogd("");
            break;
        }
    }
    if (found) {
        if (ctx.error_code) {
            //说明处理出错了。
            return send_error(conn, ctx.error_code, ctx.error_message, id);
        } else {
            //正常处理了
            if (id != NULL) {
                return send_result(conn, ret, id);
            } else {
                //这个是notification，不能回复。
                return ret;
            }

        }
    } else {
        mylogd("not found the command");
        //没有找到
        return send_error(conn, JRPC_METHOD_NOT_FOUND, strdup("Method not found"), id);
    }
}


static int eval_request(struct jrcp_server *server, struct jrpc_connection *conn,
    cJSON *root)
{
    cJSON *method, *params, *id;
    method = cJSON_GetObjectItem(root, "method");

    if (!((method != NULL) && (method->type == cJSON_String))) {
        // method解析不对，返回错误。
        myloge("method is not string or method is null");
        goto fail;
    }
    // get params
    // params也是可选的。
    params = cJSON_GetObjectItem(root, "params");
    if (params == NULL) {
        //这个是正常的。
    } else if ((
        (params != NULL) &&
        ((params->type == cJSON_Array ) || (params->type == cJSON_Object))
    )) {
       //这个也是正常的。
    } else {
        //这里才是不正常的。
        mylogd("params is not right");
        goto fail;
    }

    // get id
    id = cJSON_GetObjectItem(root, "id");
    // id可以没有，如果有的话，那么必须是string或number类型。
    if (id == NULL) {
        //这个是正常的。
        //这个说明是通知。
    } else if ((
        (id != NULL) &&
        ((id->type == cJSON_String ) || (id->type == cJSON_Number))
    )) {
       //这个也是正常的。
       //这个有id，说明是一个request。

    } else {
        //这里才是不正常的。
        mylogd("id is not right");
        goto fail;
    }
    //到这里才说明各个部分都是正常的。
    cJSON *id_copy = NULL;
    //如果有id
    if (id != NULL) {
        //把id拷贝一份，为什么要拷贝？因为我们会在reply里使用，会在处理函数里进行free操作。
        if (id->type == cJSON_String) {
            id_copy = cJSON_CreateString(id->valuestring);
        } else {
            id_copy = cJSON_CreateNumber(id->valueint);
        }
    }
    return invoke_procedure(server, conn, method->valuestring, params, id_copy);
fail:
    send_error(conn, JRPC_INVALID_REQUEST, strdup("request is invalid"), NULL);
    return -1;
}

static void connection_cb(struct ev_loop *loop, ev_io *w, int revents)
{
    struct jrpc_connection *conn;
    struct jrpc_server *server = (struct jrpc_server *)w->data;
    size_t bytes_read = 0;
    conn = (struct jrpc_connection *)w;
    int fd = conn->fd;
    mylogd("pos:%d", conn->pos);
    if (conn->pos == (conn->buffer_size-1)) {
        //注意下面这里是*=，长度值就已经改了。
        char *new_buffer = realloc(conn->buffer, conn->buffer_size *= 2);
        if (new_buffer == NULL) {
            myloge("malloc fail");
            return close_connection(loop, w);
        }
        conn->buffer = new_buffer;
        memset(conn->buffer + conn->pos, 0, conn->buffer_size - conn->pos);
    }
    int max_read_size = conn->buffer_size - conn->pos - 1;
    bytes_read = read(fd, conn->buffer + conn->pos, max_read_size);
    mylogd("read len:%d", bytes_read);
    if (bytes_read < 0) {
        //这个是read出错。
        myloge("read conn fd fail");
        return close_connection(loop, w);
    } else if (bytes_read == 0) {
        //这个是client关闭连接的情况
        mylogd("client close conn, so we should close conn too");
        return close_connection(loop, w);
    } else {
        //这个是正常情况
        cJSON *root = NULL;
        char *end_ptr = NULL;
        conn->pos += bytes_read;
        //这里还是从buffer最开始解析。
        root = cJSON_Parse_Stream(conn->buffer, &end_ptr);
        // mylogd("whole buffer:%s", conn->buffer);
        // mylogd("buffer content len:%d", strlen(conn->buffer));
        // mylogd("end_ptr:%p", end_ptr);
        if (root == NULL) {
            //说明解析出错了。
            //看一下有没有解析完所有的buffer，
            //如果没有，等待后面的数据。
            //否则就是真的出错了。
            mylogd("end_ptr:%p, buffer:%p, pos:%d", end_ptr, conn->buffer, conn->pos);
            if (end_ptr != (conn->buffer + conn->pos)) {
                mylogd("invalid json received");
                //向client发送错误提示。
                send_error(conn, JRPC_PARSE_ERROR,
                    strdup("Parse error, invalid json string"), NULL);
                //需要关闭连接吗？罪不至死吧。（可能只是发错了数据而已，重新发送就好了嘛。
                //不用断开连接。但是这个时候buffer里的内容不对，需要把buffer复位一下。
                //否则后续都不能正常进行解析了。
                memset(conn->buffer, 0, conn->buffer_size);
                conn->pos = 0;
            }
        } else {
            //这个是正常的分支
            {//先打印一下。
                char *s = cJSON_Print(root);
                mylogd("received json:");
                printf(s);
                free(s);
            }
            mylogd("root->type:%d", root->type);
            //只有object类型才处理
            //其实Array类型也可以处理。那就是批处理。我先不支持吧。
            if (root->type == cJSON_Object) {
                eval_request(server, conn, root);
            }
            //把已经处理过的内容扔掉
            //这里为什么要+2？
            //mylogd("strlen(end_ptr):%d", strlen(end_ptr));
            //mylogd("end_ptr content:%s", end_ptr);
            memmove(conn->buffer, end_ptr, strlen(end_ptr)+2);
            conn->pos = strlen(end_ptr);
            memset(conn->buffer + conn->pos, 0, conn->buffer_size - conn->pos - 1);
            cJSON_Delete(root);
        }
    }
}
static void accept_cb(struct ev_loop *loop, ev_io *w, int revents)
{
    struct jrpc_connection *connection_watcher;
    connection_watcher = malloc(sizeof(*connection_watcher));
    struct jrpc_server *server = (struct jrpc_server *)w->data;
    socklen_t size = 0;
    struct sockaddr_in clientaddr;


    connection_watcher->fd = accept(w->fd, (struct sockaddr *)&clientaddr, &size);
    if (connection_watcher->fd < 0) {
        myloge("accept fail");
        goto fail;
    }
    mylogd("get connection ");
    // 把connection加入到server中进行管理
    // 方便在需要的时候，遍历发送广播给所有的client
    int i;
    for (i=0; i<JRPC_SERVER_CLIENT_NUM; i++) {
        if (server->conns[i] == NULL) {
            server->conns[i] = connection_watcher;
            server->conn_count ++;
            break;
        }
    }
    mylogd("conn count:%d", server->conn_count);
    if (i >= JRPC_SERVER_CLIENT_NUM) {
        //说明没有找到空闲的了。
        //也关闭。
        mylogd("all conns is used now");
        goto fail;
    }
    ev_io_init(&connection_watcher->io, connection_cb, connection_watcher->fd,
        EV_READ);
    connection_watcher->io.data = w->data;
    connection_watcher->buffer_size = 1500;
    connection_watcher->buffer = malloc(1500);
    if (connection_watcher->buffer == NULL) {
        goto fail;
    }
    memset(connection_watcher->buffer, 0, 1500);
    connection_watcher->pos = 0;
    ev_io_start(loop, &connection_watcher->io);

    mylogd("accept ok, fd:%d", connection_watcher->fd);
    return;
fail:
    if (connection_watcher->fd > 0) {
        mylogd("close accepted fd");
        close(connection_watcher->fd);
    }

    if (connection_watcher) {
        free(connection_watcher);
    }

}
static int __jrpc_server_start(struct jrpc_server *server)
{
    int sockfd = -1;
    int ret = -1;
    struct sockaddr_in sockaddr = {0};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(server->port_number);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        myloge("create socket fail");
        return -1;
    }
    int yes = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&yes, sizeof(yes) );
    if (ret < 0) {
        myloge("setsockopt fail");
        goto fail;
    }

    ret = bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (ret < 0) {
        myloge("bind fail");
        goto fail;
    }

    ret = listen(sockfd, 5);
    if (ret < 0) {
        myloge("listen fail");
        goto fail;
    }

    ev_io_init(&server->listen_watcher, accept_cb, sockfd, EV_READ);
    server->listen_watcher.data = server;
    mylogd("server->loop:%p", server->loop);
    ev_io_start(server->loop, &server->listen_watcher);

    return 0;
fail:
    if (sockfd > 0) {
        close(sockfd);
    }
    return -1;
}
int jrpc_server_init_with_ev_loop(struct jrpc_server *server, int port, struct ev_loop *loop)
{
    memset(server, 0, sizeof(*server));
    server->loop = loop;
    server->port_number = port;
    server->conn_count = 0;
    int i;
    for (i=0; i<JRPC_SERVER_CLIENT_NUM; i++) {
        server->conns[i] = NULL;
    }
    server->procedure_count = 0;
    return __jrpc_server_start(server);
}

int jrpc_server_init(struct jrpc_server *server, int port)
{
    loop = ev_default_loop(0);
    return jrpc_server_init_with_ev_loop(server, port, loop);
}

int jrpc_server_register_procedure(struct jrpc_server *server,
     char *name, jrpc_function func, void *data)
{
    struct jrpc_procedure *proc = jrpc_procedure_create( name, func, data);
    if (!proc) {
        myloge("malloc fail");
        goto fail;
    }
    //找一下当前的procs的空位
    int i = 0;
    for (i=0; i<JRPC_SERVER_PROC_NUM; i++) {
        if (server->procedures[i] == NULL) {
            server->procedures[i] = proc;
            break;
        }
    }
    if (i >= JRPC_SERVER_PROC_NUM) {
        myloge("too many proc is registered");
        goto fail;
    }

    server->procedure_count ++;
    mylogd("%s cmd register ok, server->procedures[i]->name:%s", name, server->procedures[i]->name);
    return 0;
fail:
    if (proc) {
        free(proc);
    }
    return -1;
}


void jrpc_server_run(struct jrpc_server *server)
{
    ev_run(server->loop, 0);
}

void jrpc_server_stop(struct jrpc_server *server)
{
    ev_break(server->loop, EVBREAK_ALL);
}



void jrpc_server_destroy(struct jrpc_server *server)
{
    if (!server) {
        myloge("server is NULL");
        return;
    }
    int i;
    for (i=0; i<server->procedure_count; i++) {
        jrpc_procedure_destroy(server->procedures[i]);
    }
    //需要释放server指针。
    free(server);
}




int jrpc_server_broadcast(struct jrpc_server *server, char *key, char *value)
{
    int i;

    // 把namespace和value转换成一个cjson对象。
    // 然后打印成json字符串
    // 解析过程还是跟tp_get里的类似。
    cJSON *root = cJSON_CreateObject();
    cJSON *method_json = cJSON_CreateString("method");
    cJSON_AddItemToObject(root, "method", method_json);
    cJSON *params_json = cJSON_CreateObject();
    cJSON *key_json = cJSON_CreateString(value);
    cJSON_AddItemToObject(params_json, key, key_json);
    cJSON_AddItemToObject(root, "params", params_json);
    char *str = cJSON_Print(root);
    mylogd("broadcast string:%s", str);
    cJSON_Delete(root);
    for (i=0; i<JRPC_SERVER_CLIENT_NUM; i++) {
        if (server->conns[i] != NULL) {
            mylogd("send to client[%d]", i);
            send(server->conns[i]->fd, str, strlen(str), 0);
        }
    }
    free(str);
    return 0;
}

struct jrpc_server *jrpc_server_create(struct ev_loop *loop, int port)
{
    struct jrpc_server * server = malloc(sizeof(*server));
    int ret = -1;
    if (!server) {
        myloge("malloc fail");
        goto fail;
    }
    // 看loop是否是null，如果是，使用default loop
    if (loop == NULL) {
        ret = jrpc_server_init(server, port);
    } else {
        ret = jrpc_server_init_with_ev_loop(server, port, loop);
    }
    if (ret < 0) {
        myloge("server init fail");
        goto fail;
    }
    return server;
fail:
    if (server) {
        free(server);
    }
    return NULL;
}
