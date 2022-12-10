#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/listener.h"
#include "event2/util.h"
#include <netinet/in.h>

#include <arpa/inet.h>

#include "mylog.h"
#include <sys/socket.h>

static void conn_readcb(struct bufferevent* bev, void* data)
{
    // 拿到input的缓冲区
    struct evbuffer* input = bufferevent_get_input(bev);
    struct evbuffer* output = bufferevent_get_output(bev);
    char* line = NULL;
    size_t n = 0;
    while (1) {
        line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF);
        mylogd("readln return :%s", line);
        if (line == NULL) {
            break;
        }
        evbuffer_add(output, line, n);
        evbuffer_add(output, "\n", 1);
        free(line);
    }
    // 然后往外写。
    char buf[1024] = { 0 };
    while (1) {
        if (evbuffer_get_length(input) == 0) {
            break;
        }
        int n = evbuffer_remove(input, buf, sizeof(buf));
        evbuffer_add(output, buf, n);
    }
    evbuffer_add(output, "\n", 1);
}

static void conn_writecb(struct bufferevent* bev, void* data)
{
    // 拿到outpu的缓冲区
    struct evbuffer* output = bufferevent_get_output(bev);
    mylogd("output content len: %d", evbuffer_get_length(output));
    // while (1) {
    //     if (evbuffer_get_length(output) == 0) {
    //         break;
    //     }
    // }
    // mylogd("write cb");
}

static void conn_eventcb(struct bufferevent* bev, short events, void* data)
{
    if (events & BEV_EVENT_EOF) {
        // 说明连接关闭了。
        mylogd("connection closed");

    } else if (events & BEV_EVENT_ERROR) {
        // 连接出错了。
        myloge("connection error:%s", strerror(errno));
    }
    bufferevent_free(bev);
}

static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
    struct sockaddr* sa, int socklen, void* userdata)
{
    struct event_base* base = userdata;
    struct bufferevent* bev;
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        myloge("create buffer event fail");
        // 退出循环
        event_base_loopbreak(base);
        return;
    }
    // 设置每一个连接处理的函数。
    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_READ);

    // 每收到一个连接，给client发送一个欢迎信息。
    bufferevent_write(bev, "welcome to server\n", strlen("welcome to server\n"));
}

static void signal_cb(evutil_socket_t sig, short events, void* data)
{
    struct event_base* base = data;
    struct timeval delay = { 0, 500 };
    mylogd("get SIGINT, would exit in 500ms");
    event_base_loopexit(base, &delay);
}

int main(int argc, char const* argv[])
{
    struct event_base* base;
    struct evconnlistener* listener;
    struct event* signal_event;

    struct sockaddr_in sin = { 0 };
    base = event_base_new();
    if (!base) {
        myloge("event_base_new fail");
        goto fail;
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(2345);
    listener = evconnlistener_new_bind(base, listener_cb, (void*)base,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        myloge("create listener fail");
        goto fail;
    }
    signal_event = evsignal_new(base, SIGINT, signal_cb, base);
    if (!signal_event) {
        myloge("create signal event fail");
        goto fail;
    }
    event_add(signal_event, NULL);
    // 开始循环
    event_base_dispatch(base);
    // 循环退出后，清理资源
    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);

    mylogd("loop exit");
    return 0;
fail:
    return -1;
}
