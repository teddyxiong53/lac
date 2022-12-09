#include "jsonrpc_client.h"

#include <stdio.h>
#include <stdlib.h>

#include "mylog.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>

static int setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

static int jrpc_client_init(struct jrpc_client* client, char* host, int port)
{
    client->sockfd = -1;
    client->server_ip = strdup(host);
    client->port = port;
    client->send_buf_size = 1024;
    client->send_buf = malloc(client->send_buf_size);
    if (client->send_buf == NULL) {
        myloge("malloc fail");
        goto fail;
    }
    client->recv_buf_size = 1024;
    client->recv_buf = malloc(client->recv_buf_size);
    if (client->recv_buf == NULL) {
        myloge("malloc fail");
        goto fail;
    }
    client->loop = ev_default_loop(0);
    return 0;
fail:
    return -1;
}

struct jrpc_client* jrpc_client_create(char* host, int port)
{
    struct jrpc_client *client = malloc(sizeof(*client));
    if (!client) {
        myloge("malloc fail");
        goto fail;
    }
    int ret = jrpc_client_init(client, host, port);
    if (ret < 0) {
        mylogd("jrpc_client_init fail");
        goto fail;
    }
    return client;
fail:
    if (client) {
        free(client);
    }
    return NULL;
}



static void process_recv_msg(struct jrpc_client* client)
{
    // 从recv_buf里取出数据
    // 进行json解析。
    // 判断id跟cur_id是不是一样。
    // 如果一样，说明是自己发送的数据的处理了结果。
    // 否则说明是server主动发来的。
    // 先只是打印出来看看吧。
    mylogd("recv buf:%s", client->recv_buf);
    // 清空recv buf
    memset(client->recv_buf, 0, strlen(client->recv_buf));
}

static void send_cb(struct ev_loop* loop, ev_io* w, int revents)
{
    mylogd("revents:0x%x", revents);
    struct jrpc_client* client = (struct jrpc_client*)w->data;
    int ret = 0;
    if (revents | EV_READ) {
        // 收到数据，有两种可能：
        // 1. 自己发送的命令的返回值。
        // 2. server主动发过来的东西。
        // 那我是不是应该区分一下呢？
        // 如何区分？
        // 在client里加一个标志，表示自己正在发送数据。
        // 就叫cur_cmd[128] 用数组来存放。
        // 如果这个里面有内容，则说明收到的是这个命令的返回。
        // 那用id来标识，岂不是更好？
        // 因为是client端控制id是不是带上了。
        // 那我就规定一定要带id。
        // id就用一个递增的int类型。
        // 用cur_id来存放就好了。
        // 处理完之后，把cur_id赋值为-1
        // 如果收到的数据的id，跟cur_id不同，说明是server主动发过来的。
        ret = recv(client->sockfd, client->recv_buf, client->recv_buf_size - 1, 0);
        if (ret < 0) {
            //说明接收出错了
            myloge("recv fail");
            //先不做处理吧。
        } else if (ret == 0) {
            //说明对端关闭了socket
            myloge("server close socket, so I close my socket too");
            ev_io_stop(client->loop, &(client->watcher));
            close(client->sockfd);
            //作为client，关闭了socket，也没有什么存在意义了
            //直接退出吧。
            exit(-1);
        } else {
            //这个才是成功。
            process_recv_msg(client);
        }
    }
}



static void eventfd_cb(struct ev_loop* loop, ev_io* w, int revents)
{
    mylogd("eventfd trigger");
    struct jrpc_client* client = (struct jrpc_client*)w->data;
    mylogd("ready to write");
    int ret;
    ret = send(client->sockfd, client->send_buf, strlen(client->send_buf), 0);
    if (ret < 0) {
        myloge("send fail");
        return;
    }
    //情况发送buf
    memset(client->send_buf, 0, strlen(client->send_buf));
}

int jrpc_client_connect_server(struct jrpc_client* client)
{
    struct sockaddr_in serveraddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        myloge("create socket fail");
        goto fail;
    }
    int ret = 0;
    // ret = setnonblock(sockfd);
    // if (ret < 0) {
    //     myloge("set nonblock fail");
    //     goto fail;
    // }
    ret = inet_pton(AF_INET, client->server_ip, &serveraddr.sin_addr);
    if (ret < 0) {
        myloge("inet_pton fail");
        goto fail;
    }
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(client->port);

    //可以开始连接了。
    ret = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        myloge("connect fail");
        //非阻塞方式，这里必然是fail的。所以要忽略这个错误。
        perror("connect fail");
        goto fail;
    }

    client->sockfd = sockfd;
    mylogd("sockfd:%d", client->sockfd);

    // 默认只监听EV_READ。需要发送的时候，才监听EV_WRITE。
    ev_io_init(&(client->watcher), send_cb, client->sockfd, EV_READ);
    //需要把这个数据保存，这样才能在回调函数里使用client里的东西。
    client->watcher.data = client;
    ev_io_start(client->loop, &(client->watcher));

    mylogd("connect ok");
    return 0;
fail:
    if (sockfd > 0) {
        close(sockfd);
    }
    return -1;
}

void jrpc_client_destroy(struct jrpc_client* client)
{
    if (!client) {
        return;
    }
    if (client->server_ip) {
        free(client->server_ip);
        client->server_ip = NULL;
    }
    if (client->send_buf) {
        free(client->send_buf);
        client->send_buf = NULL;
    }
    if (client->recv_buf) {
        free(client->recv_buf);
        client->recv_buf = NULL;
    }
    free(client);
}

void jrpc_client_run(struct jrpc_client* client)
{
    ev_run(client->loop, 0);
}

void jrpc_client_stop(struct jrpc_client* client)
{
    ev_break(client->loop, EVBREAK_ALL);
}


/*
    提供什么样的对外接口，才能使用更加简单呢？
    是不是可以对cmd进行一些抽象和封装呢？
    如果直接让用户传递cJSON指针，那就需要用户自己拼接构造一个cJSON对象。
    能不能更简单一点？
    可以提供一个简单的接口，例如只需要传递一个命令名字的字符串。
    这个对于那些没有参数的情况很有用。（这种情况其实还是挺多的）。
    另外，为了应对各种复杂度的情况，把完整的接口还是需要提供给用户。
    让用户自己决定选择哪个。
    cmd的cjson需要用户自己销毁。
*/
int jrpc_client_send_cmd(struct jrpc_client* client, cJSON* cmd)
{

    int ret = -1;
    //把cmd print成json字符串，拷贝给send_buf。
    char* str = cJSON_Print(cmd);
    mylogd("send json:%s", str);
    //直接这里进行发送。
    ret = send(client->sockfd, str, strlen(str), 0);
    free(str); //必须马上free掉。
    if (ret < 0) {
        myloge("send fail");
        return -1;
    }
    return 0;
}

int jrpc_client_get_unique_id()
{
    static unsigned int id = 0;
    static pthread_mutex_t lock;
    pthread_mutex_lock(&lock);
    id++;
    if (id < 0) {
        id = 1;
    }
    pthread_mutex_unlock(&lock);
    return id;
}

/*
    这个是传递string参数的接口。
    cmd可以是一个普通的简单字符串。
    也可以是一个合法的json字符串。
    如果是json字符串。那么就构造传递。
    如果是普通字符串（里面没有大括号），那么就当成只有method的notification来构造。
*/
int jrpc_client_send_cmd_str(struct jrpc_client* client, char* cmd)
{

    cJSON* root = NULL;
    int ret = -1;
    //查看cmd里有没有{和}这2个字符。如果有，说明是一个json字符串。
    if ((strstr(cmd, "{") != NULL) && (strstr(cmd, "}") != NULL)) {
        // 尝试解析json字符串。
        root = cJSON_Parse(cmd);
        if (root == NULL) {
            myloge("cmd % is not a valid json string", cmd);
            return -1;
        }
    } else {
        //说明是普通字符串。需要构造一个notification。
        root = cJSON_CreateObject();
        cJSON* method_json = cJSON_CreateString(cmd);
        cJSON_AddItemToObject(root, "method", method_json);
    }
    ret = jrpc_client_send_cmd(client, root);
    //自己来销毁cjson
    cJSON_Delete(root);
    return ret;
}
