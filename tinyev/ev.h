#ifndef _TINYEV_EV_H_
#define _TINYEV_EV_H_

typedef double ev_tstamp;

#include <signal.h>
#define EV_ATOMIC_T sig_atomic_t volatile

struct ev_loop;

#define EV_MAXPRI -2
#define NUMPRI  1

typedef ev_watcher *W;
typedef ev_watcher_list *WL;
typedef ev_watcher_time *WT;


enum {
    EVBREAK_CANCEL = 0,//撤销unloop
    EVBREAK_ONE = 1,//unloop一次。
    EVBREAK_ALL = 2 //unloop 所有的loop
}
typedef struct {
    W w;
    int events;
} APPENDNIG;

typedef struct {
    WL head;
    unsigned char events;//监听的事件
    unsigned char reify;//具体化
    unsigned char emask;//对epoll这个backend来说，存储真正的kernel mask
    unsigned char unused;
    unsigned int egen;//给epoll用的，统计epoll的bug产生次数。

} ANFD;

typedef WT ANHE;

struct ev_loop {
    ev_tstamp ev_rt_now;// realtime now

    ev_tstamp now_floor;
    ev_tstamp mn_now;// monotonic now
    ev_tstamp rtmn_diff;//realtime monotonic diff

    // for reverse feeding of events
    W * rfeeds;
    int rfeedmax;
    int rfeedcnt;

    APPENDNIG *pendings[NUMPRI];
    int pendingmax[NUMPRI];
    int pendingcnt[NUMPRI];

    int pendingpri;
    ev_prepare pending_w;

    ev_tstamp io_blocktime;
    ev_tstamp timeout_blocktime;

    int backend;//表示哪一种backend。我固定为epoll。所以这个参数对我没有意义。
    int activecnt;
    EV_ATOMIC_T loop_done;

    int backend_fd;
    ev_tstamp backend_mintime;

    void (*backend_modify)(struct evloop *loop, int fd, int oev, int nev);
    void (*backend_poll)(struct evloop *loop, ev_tstamp timeout);

    ANFD * anfds;
    int anfdmax;

    int evpipe[2];
    ev_io pipe_w;
    EV_ATOMIC_T pipe_write_wanted;
    EV_ATOMIC_T pipe_write_skipped;

    pid_t curpid;
    char postfork;//?

    //只使用epoll的
    struct epoll_event *epoll_events;
    int epoll_eventmax;
    int *epoll_eperms;
    int epoll_epermcnt;
    int epoll_epermmax;

    int *fdchanges;
    int fdchangemax;
    int fdchangecnt;

    ANHE *timers;
    int timermax;
    int timercnt;

    ANHE *periodics;
    int periodicmax;
    int periodiccnt;

    ev_idle **idles[NUMPRI];
    int idlemax[NUMPRI];
    int idlecnt[NUMPRI];
    int idleall;

    struct ev_prepare **prepares;
    int preparemax;
    int preparecnt;

    struct ev_check **checks;
    int checkmax;
    int checkcnt;

    struct ev_fork ** forks;
    int forkmax;
    int forkcnt;

    struct ev_cleanup **cleanups;
    int cleanupmax;
    int cleanupcnt;

    EV_ATOMIC_T async_pending;
    struct ev_async **asyncs;
    int asyncmax;
    int asynccnt;

    EV_ATOMIC_T sig_pending;
    int sigfd;
    ev_io sigfd_w;
    sigset_t sigfd_set;

    unsigned int origflags;// orignal loop flags

    unsigned int loop_count;
    unsigned int loop_depth;

    void *userdata;
};

/*
    提前声明这些结构体类型。方便定义cb类型使用。
*/
struct ev_watcher;
struct ev_watcher_list;
struct ev_watcher_time;

struct ev_io;
struct ev_timer ;
struct ev_periodic;
struct ev_signal;
struct ev_child ;
struct ev_stat;
struct ev_idle;
struct ev_prepare;
struct ev_check;
struct ev_fork;
struct ev_cleanup;
struct ev_async;

typedef void (*ev_watcher_cb_t)(struct ev_loop *loop, struct ev_watcher *w, int revents);
typedef struct ev_watcher {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_watcher *w, int revents);

} ev_watcher;

typedef void (*ev_watcher_list_cb_t)(struct ev_loop *loop, struct ev_watcher_list *w, int revents);
typedef struct ev_watcher_list {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_watcher_list *w, int revents);

    struct ev_watcher_list *next;
} ev_watcher_list;

typedef void (*ev_watcher_time_cb_t)(struct ev_loop *loop, struct ev_watcher_time *w, int revents);
typedef struct ev_watcher_time {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_watcher_time *w, int revents);
    ev_tstamp at;
} ev_watcher_time;

typedef void (*ev_io_cb_t)(struct ev_loop *loop, struct ev_io *w, int revents);
typedef struct ev_io {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_io *w, int revents);

    struct ev_watcher_list *next;
    int fd;
    int events;

} ev_io;

typedef void (*ev_timer_cb_t)(struct ev_loop *loop, struct ev_timer *w, int revents);
typedef struct ev_timer {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_timer *w, int revents);
    ev_tstamp at;
    ev_tstamp repeat;
} ev_timer;

typedef void (*ev_periodic_cb_t)(struct ev_loop *loop, struct ev_periodic *w, int revents);
typedef ev_tstamp (*ev_periodic_reschedule_cb_t)(struct ev_periodic *w, ev_tstamp now)
typedef struct ev_periodic {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_periodic *w, int revents);
    ev_tstamp at;
    ev_tstamp offset;
    ev_tstamp interval;
    ev_periodic_reschedule_cb_t reschedule_cb;
} ev_periodic;

typedef void (*ev_signal_cb_t)(struct ev_loop *loop, struct ev_signal *w, int revents);
typedef struct ev_signal {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_signal *w, int revents);

    struct ev_watcher_list *next;
    int signum;
} ev_signal;

typedef void (*ev_child_cb_t)(struct ev_loop *loop, struct ev_child *w, int revents);
typedef struct ev_child {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_child *w, int revents);

    struct ev_watcher_list *next;
    int flags;
    int pid;
    int rpid;
    int rstatus;
} ev_child;

typedef struct stat ev_statdata;

typedef void (*ev_stat_cb_t)(struct ev_loop *loop, struct ev_stat *w, int revents);
typedef struct ev_stat {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_stat *w, int revents);

    struct ev_watcher_list *next;
    ev_timer timer;     /* private */
    ev_tstamp interval; /* ro */
    const char *path;   /* ro */
    ev_statdata prev;   /* ro */
    ev_statdata attr;   /* ro */

    int wd; /* wd for inotify, fd for kqueue */
} ev_stat;

typedef void (*ev_idle_cb_t)(struct ev_loop *loop, struct ev_idle *w, int revents);
typedef struct ev_idle {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_idle *w, int revents);

} ev_idle;

typedef void (*ev_prepare_cb)(struct ev_loop *loop, struct ev_prepare *w, int revents);
typedef struct ev_prepare {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_prepare *w, int revents);

} ev_prepare;

typedef void (*ev_check_cb_t)(struct ev_loop *loop, struct ev_check *w, int revents);
typedef struct ev_check {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_check *w, int revents);

} ev_check;

typedef void (*ev_fork_cb_t)(struct ev_loop *loop, struct ev_fork *w, int revents);
typedef struct ev_fork {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_fork *w, int revents);

} ev_fork;

typedef void (*ev_cleanup_cb_t)(struct ev_loop *loop, struct ev_cleanup *w, int revents);
typedef struct ev_cleanup {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_cleanup *w, int revents);

} ev_cleanup;

typedef void (*ev_async_cb_t)(struct ev_loop *loop, struct ev_async *w, int revents);
typedef struct ev_async {
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_async *w, int revents);

    EV_ATOMIC_T sent;
} ev_async;

/**
 * @brief event mask
 *
 */
enum {
    EV_UNDEF = (int)0xffffffff,
    EV_NONE = 0x00,
    EV_READ = 0x01,
    EV_WRITE = 0X02,
    EV__IOFDSET = 0X80,//internal use
    EV_IO = EV_READ,
    EV_TIMER = 0X0100,
    EV_PERIODIC = (1<<9),
    EV_SIGNAL = (1<<10),
    EV_CHILD = (1<<11),
    EV_STAT = (1<<12),
    EV_IDLE = 1<<13,
    EV_PREPARE = 1<<14,
    EV_CHECK = 1<<15,
    EV_EMBED = 1<<16,
    EV_FORK = 1<<17,
    EV_CLEANUP = 1<<18,
    EV_ASYNC = 1<<19,
    EV_CUSTOM = 0X01000000,
    EV_ERROR = (int)0x80000000
};

/**
 * @brief flags
 *
 */
enum {
    EVFLAG_AUTO = 0,
    EVFLAG_NOEV = 0X01000000

};


/**
 * @brief 这个只能用宏的方式来做了。因为要传递不同的ev类型。就相当于c++的模板了。
 *
 */
#define ev_init(ev_, cb_) do { \
    ev_->active = 0; \
    ev_->pending = 0; \
    ev_->cb = cb_; \
} while (0)

/******************** set  ***********************/
static inline void ev_io_set(ev_io *ev, int fd, int events)
{
    ev->fd = fd;
    ev->events = events | EV__IOFDSET;

}

static inline void ev_timer_set(ev_timer *ev, ev_tstamp after, ev_tstamp repeat)
{
    ev->at = after;
    ev->repeat = repeat;
}

static inline void ev_periodic_set(ev_periodic *ev, ev_tstamp offset,
    ev_tstamp interval, ev_periodic_reschedule_cb_t cb)
{
    ev->offset = offset;
    ev->interval = interval;
    ev->reschedule_cb = cb;
}

static inline void ev_signal_set(ev_signal *ev, int signum)
{
    ev->signum = signum;
}

/******************** init  ***********************/
static inline void ev_io_init(ev_io *ev, ev_io_cb_t cb, int fd, int events)
{
    ev_init(ev, cb);
    ev_io_set(ev, fd, events);
}

static inline void ev_timer_init(ev_timer *ev, ev_timer_cb_t cb, ev_tstamp after, ev_tstamp repeat)
{
    ev_init(ev, cb);
    ev_timer_set(ev, after, repeat);
}

static inline void ev_periodic_init(ev_periodic *ev, ev_periodic_cb_t cb, ev_tstamp offset,
    ev_tstamp interval, ev_periodic_reschedule_cb_t rcb)
{
    ev_init(ev, cb);
    ev_periodic_set(ev, offset, interval, rcb);
}

static inline void ev_signal_init(ev_signal *ev, ev_signal_cb_t cb, int signum)
{
    ev_init(ev, cb);
    ev_signal_set(ev, signum);
}

#define ev_is_pending(ev) \
    (0 + ((ev_watcher *)(void *)(ev))->pending)

#define ev_is_active(ev)                     (0 + ((ev_watcher *)(void *)(ev))->active)



/************** start/stop************/
void ev_io_start(struct ev_loop *loop, ev_io *w);
void ev_io_stop(struct ev_loop *loop, ev_io *w);


#endif
