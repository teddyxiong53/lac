#ifndef __TINYEV_H__
#define __TINYEV_H__

/* 定义事件类型 */
#define TINYEV_IO_EVENT   0x01 /* IO 事件 */
#define TINYEV_TIMER_EVENT   0x02 /* 定时器事件 */
#define TINYEV_SIGNAL_EVENT   0x04 /* 信号事件 */

/* 定义事件处理函数类型 */
typedef void (*tinyev_event_cb)(int fd, short events, void *arg);

/* 定义事件结构体 */
typedef struct tinyev_event_s {
    int fd; /* 文件描述符 */
    short events; /* 事件类型 */
    tinyev_event_cb cb; /* 事件处理函数 */
    void *arg; /* 用户数据 */
} tinyev_event_t;

/* 定义事件循环结构体 */
typedef struct tinyev_loop_s {
    int poll_fd; /* poll 文件描述符 */
    tinyev_event_t *events; /* 事件数组 */
    int nevents; /* 事件数量 */
    int max_events; /* 事件数组的最大长度 */
} tinyev_loop_t;

/* 初始化事件循环 */
int tinyev_init(tinyev_loop_t *loop, int max_events);

/* 向事件循环中添加事件 */
int tinyev_add_event(tinyev_loop_t *loop, tinyev_event_t *event);

/* 从事件循环中删除事件 */
int tinyev_del_event(tinyev_loop_t *loop, tinyev_event_t *event);

/* 运行事件循环 */
int tinyev_run(tinyev_loop_t *loop);

/* 停止事件循环 */
int tinyev_stop(tinyev_loop_t *loop);

/* 创建 IO 事件 */
void tinyev_io_init(tinyev_event_t *event, int fd, short events, tinyev_event_cb cb, void *arg);

/* 创建定时器事件 */
void tinyev_timer_init(tinyev_event_t *event, tinyev_event_cb cb, void *arg);

/* 创建信号事件 */
void tinyev_signal_init(tinyev_event_t *event, int signum, tinyev_event_cb cb, void *arg);

#endif // __TINYEV_H__
