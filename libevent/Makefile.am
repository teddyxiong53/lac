lib_LTLIBRARIES = libevent.la

libevent_la_SOURCES = \
    buffer.c \
    bufferevent.c \
    bufferevent_filter.c \
    bufferevent_pair.c \
    bufferevent_ratelim.c \
    bufferevent_sock.c \
    event.c \
    evmap.c \
    evthread.c \
    evutil.c \
    evutil_rand.c \
    evutil_time.c \
    listener.c \
    log.c \
    strlcpy.c \
    select.c \
    poll.c \
    epoll.c \
    signal.c \
    evdns.c \
    event_tagging.c \
    evrpc.c \
    http.c

AM_CFLAGS = \
	-I$(top_srcdir)/include \
    -I$(top_srcdir)/libevent/include
AM_CFLAGS += -g -O0 -rdynamic -funwind-tables