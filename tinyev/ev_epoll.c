#include "ev.h"
#include <sys/epoll.h>

/**
 * @brief
 *
 * @param loop
 * @param fd
 * @param oev old ev
 * @param nev  new ev
 */
static void epoll_modify(struct ev_loop *loop, int fd, int oev, int nev)
{
    struct epoll_event ev;
    unsigned char oldmask;

    if (!new) {
        return;
    }
    oldmask = loop->anfds[fd].emask;
    loop->anfds[fd].emask = nev;

}

static void epoll_poll(struct ev_loop *loop, ev_tstamp timeout)
{

}
/**
 * @brief
 *
 * @param loop
 * @param flags
 * @return int 0表示失败。1表示成功。
 */
int epoll_init(struct ev_loop *loop, int flags)
{
    loop->backend_fd = epoll_create1(EPOLL_CLOEXEC);
    if (loop->backend_fd < 0) {
        return 0;
    }
    loop->backend_mintime = 1e-3;//有时候epoll会提前返回。这个是兜底。
    loop->backend_modify = epoll_modify;
    loop->backend_poll = epoll_poll;
    loop->epoll_eventmax = 64;
    int size = sizeof(struct epoll_event)*loop->epoll_epermmax;
    loop->epoll_events = (struct epoll_event *)malloc();
    memset(loop->epoll_events, 0, size);
    return 1;//本来是返回backend对应的mask值。但是我只有epoll。所以直接返回1.
}