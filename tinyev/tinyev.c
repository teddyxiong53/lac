#include "tinyev.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>
#include <tinyev_poll.h>

/* 初始化事件循环 */
int tinyev_init(tinyev_loop_t *loop, int max_events) {
    /* 创建 poll 文件描述符 */
    loop->poll_fd = poll_create();
    if (loop->poll_fd < 0) {
        return -1;
    }

    /* 分配事件数组内存 */
    loop->events = (tinyev_event_t *) malloc(sizeof(tinyev_event_t) * max_events);
    if (loop->events == NULL) {
        return -1;
    }

    /* 初始化事件数组 */
    loop->nevents = 0;
    loop->max_events = max_events;

    return 0;
}


int tinyev_add_event(tinyev_loop_t *loop, tinyev_event_t *event) {
    /* 检查事件数量是否超过最大值 */
    if (loop->nevents >= loop->max_events) {
        return -1;
    }

    /* 将事件添加到事件数组中 */
    loop->events[loop->nevents] = *event;
    loop->nevents++;

    /* 更新 poll 的事件集合 */
    poll_add_fd(event->fd, event->events);

    return 0;
}

/* 从事件循环中删除事件 */
int tinyev_del_event(tinyev_loop_t *loop, tinyev_event_t *event) {
    /* 查找事件在事件数组中的位置 */
    int i;
    for (i = 0; i < loop->nevents; i++) {
        if (loop->events[i].fd == event->fd && loop->events[i].events == event->events) {
            break;
        }
    }

    /* 如果找到事件，则从事件数组中删除 */
    if (i < loop->nevents) {
        loop->nevents--;
        if (i < loop->nevents) {
            loop->events[i] = loop->events[loop->nevents];
        }
        poll_del_fd(event->fd);

        return 0;
    }

    return -1;
}

/* 运行事件循环 */
int tinyev_run(tinyev_loop_t *loop) {
    /* 循环处理事件 */
    while (1) {
        /* 等待事件 */
        int nready = poll_wait(-1);
        if (nready < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        /* 处理就绪的事件 */
        int i;
        for (i = 0; i < loop->nevents && nready > 0; i++) {
            struct pollfd *pfd = &loop->events[i];
            if (pfd->revents != 0) {
                nready--;

                /* 调用事件处理函数 */
                tinyev_event_t *event = &loop->events[i];
                event->cb(event->fd, event->events, event->arg);
            }
        }
    }

    return 0;
}

/* 停止事件循环 */
int tinyev_stop(tinyev_loop_t *loop) {
    /* 关闭 poll 文件描述符 */
    poll_close(loop->poll_fd);

    /* 释放事件数组内存 */
    free(loop->events);

    return 0;
}

/* 创建 IO 事件 */
void tinyev_io_init(tinyev_event_t *event, int fd, short events, tinyev_event_cb cb, void *arg) {
    event->fd = fd;
    event->events = events;
    event->cb = cb;
    event->arg = arg;
}

/* 创建定时器事件 */
void tinyev_timer_init(tinyev_event_t *event, tinyev_event_cb cb, void *arg) {
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    event->fd = fd;
    event->events = TINYEV_IO_EVENT;
    event->cb = cb;
    event->arg = arg;
}

/* 创建信号事件 */
void tinyev_signal_init(tinyev_event_t *event, int signum, tinyev_event_cb cb, void *arg) {
    int fd = signalfd4(-1, &signum, sizeof(signum), SFD_NONBLOCK | SFD_CLOEXEC);
    event->fd = fd;
    event->events = TINYEV_IO_EVENT;
    event->cb = cb;
    event->arg = arg;
}