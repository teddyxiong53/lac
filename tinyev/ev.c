#include "ev.h"

#define EV_ANFD_REIFY 1


static struct ev_loop default_loop_struct;
struct ev_loop *ev_default_loop_ptr = NULL;

ev_tstamp ev_time()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec/1000000;
}
ev_tstamp get_clock()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec*1e-9;
}
static void pendingcb(struct ev_loop *loop, ev_prepare *w, int revents)
{

}

static void pipecb(struct ev_loop *loop, ev_io *iow, int revents)
{
    int i;
    if (revents & EV_READ) {
        if (loop->evpipe[0] < 0) {
            //什么情况会发生这个？
            uint64_t counter;
            read(loop->evpipe[1], &counter, sizeof(counter));
        } else {
            char dummy[4];
            read(loop->evpipe[0], &dummy, sizeof(dummy));
        }
    }
    if (loop->sig_pending) {
        loop->sig_pending = 0;
        //TODO
    }
    if (loop->async_pending) {
        //TODO
    }
}

static void loop_init(struct ev_loop *loop, unsigned int flags)
{
    if (!loop->backend) {
        loop->origflags = flags;
        loop->ev_rt_now = ev_time();
        loop->mn_now = get_clock();
        loop->now_floor = loop->mn_now;
        loop->rtmn_diff = loop->ev_rt_now - loop->mn_now;

        loop->io_blocktime = 0;
        loop->timeout_blocktime = 0;
        loop->backend = 0;
        loop->backend_fd = -1;
        loop->sig_pending = 0;

        loop->async_pending = 0;

        loop->pipe_write_skipped = 0;
        loop->pipe_write_wanted = 0;
        loop->evpipe[0] = -1;
        loop->evpipe[1] = -1;

        loop->backend = epoll_init(loop, flags);

        ev_prepare_init(&(loop->pending_w), pendingcb);
        ev_init(&loop->pipe_w, pipecb);


    }
}

struct ev_loop * ev_default_loop (unsigned int flags)
{
    if (!ev_default_loop_ptr) {
        struct ev_loop *loop = ev_default_loop_ptr = &default_loop_struct;
        loop_init(loop, flags);
    }
    return ev_default_loop_ptr;
}

void ev_ref(struct ev_loop *loop)
{
    loop->activecnt ++;
}
void ev_unref(struct ev_loop *loop)
{
    loop->activecnt --;
}

void ev_start(struct ev_loop *loop, W w, int active)
{
    w->active = active;
    ev_ref(loop);
}

void ev_stop(struct ev_loop *loop, W w)
{
    ev_unref(loop);
    w->active = 0;
}
#define MALLOC_ROUND 4096


int array_nextsize(int elem, int cur, int cnt)
{
    int ncur = cur + 1;
    do {
        ncur <<= 1;
    } while (cnt > ncur);
    if (elem * ncur > MALLOC_ROUND - sizeof (void *) * 4) {
      ncur *= elem;
      ncur = (ncur + elem + (MALLOC_ROUND - 1) + sizeof (void *) * 4) & ~(MALLOC_ROUND - 1);
      ncur = ncur - sizeof (void *) * 4;
      ncur /= elem;
    }

  return ncur;
}

void *ev_realloc(void *ptr, long size)
{
    ptr = alloc(ptr, size);//这个函数是在栈上分配内存。
    if (!ptr && size) {
        myloge("alloc fail");
        abort();//很严重的问题，直接退出。
    }
    return ptr;
}

static void * array_realloc(int elem, void *base, int *cur, int cnt)
{
    *cur = array_nextsize(elem, *cur, cnt);
    return ev_realloc(base, elem * (*cur));
}

#define array_needsize(type, base, cur, cnt, init)  \
if (cnt > cur) {                                     \
    int ocur_ = cur;                                 \
    (base) = (type *)array_realloc(                  \
        sizeof(type), (base), &(cur), (cnt)          \
    );                                               \
    init((base)+(ocur_), (cur) - ocur_ );             \
}


void wlist_add (WL *head, WL elem)
{
    elem->next = *head;
    *head = elem;
}

void wlist_del(WL *head, WL elem)
{
    while (*head) {
        if (*head == elem) {
            *head = elem->next;
            break;
        }
        head = &(*head)->next;
    }
}

#define EMPTY       /* required for microsofts broken pseudo-c compiler */
#define EMPTY2(a,b) /* used to suppress some warnings */

void fd_change(struct ev_loop *loop, int fd, int flags)
{
    unsigned char reify = loop->anfds[fd].reify;
    loop->anfds[fd].reify |= flags;
    if (!reify) {
        loop->fdchangecnt++;
        array_needsize(int, loop->fdchanges, loop->fdchangemax, loop->fdchangecnt, EMPTY2);
        loop->fdchanges[loop->fdchangecnt - 1] = fd;
    }
}

void ev_io_start(struct ev_loop *loop, ev_io *w)
{
    int fd = w->fd;
    if (ev_is_active(w)) {
        mylogd("already started");
        return;
    }
    ev_start(loop, w, 1);
    array_needsize(ANFD, loop->anfds, loop->anfdmax, fd + 1, array_init_zero);
    wlist_add(&(loop->anfds[fd].head), (WL)w);

    fd_change(loop, fd, w->events & EV__IOFDSET | EV_ANFD_REIFY);
    w->events &= ~EV__IOFDSET;

}

void clear_pending(struct ev_loop *loop, W w)
{
    if (w->pending) {
        loop->pendings[0][w->pending - 1].w = (W)&(loop->pending_w);
        w->pending = 0;
    }
}
void ev_io_stop(struct ev_loop *loop, ev_io *w)
{
    clear_pending(loop, (W)w);
    if (!ev_is_active(w)) {
        return;
    }
    wlist_del(&(loop->anfds[w->fd].head), (WL)w);
    ev_stop(loop, w);
    fd_change(loop, w->fd, EV_ANFD_REIFY);

}

int ev_run(struct ev_loop *loop, int flags)
{
    loop->loop_done = EVBREAK_CANCEL;
    do {
        if (loop->curpid) {
            if (getpid() != loop->curpid) {
                loop->curpid = getpid();
                loop->postfork = 1;
            }
        }
        if (loop->postfork) {
            if (loop->forkcnt) {
                queue_events()
            }
        }
    }
}