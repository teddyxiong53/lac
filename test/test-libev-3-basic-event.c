#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/unistd.h>
#include <ev.h>
#include "mylog.h"

ev_io io_w;
ev_timer timer_w;
ev_signal signal_w;

void io_action (struct ev_loop *loop, ev_io *io_w, int e)
{
    int ret;
    char buf[1024] = {0};
    memset(buf, 0, sizeof(buf));
    mylogd("in io action");
    read(STDIN_FILENO, buf, sizeof(buf));
    mylogd("input :%s" ,buf);
    //停止监听io的。所以只能输入一次。
    ev_io_stop(loop, io_w);
}

void timer_action (struct ev_loop *loop, ev_timer *timer_w, int e)
{
    mylogd("timer action");
    ev_timer_stop(loop, timer_w);
}

void signal_action(struct ev_loop *loop, ev_signal *signal_w, int e)
{
    mylogd("signal action");
    ev_signal_stop(loop, signal_w);
    ev_break(loop, EVBREAK_ALL);
}

int main(int argc, char const *argv[])
{
    ev_io io_w;
    ev_timer tiemr_w;
    ev_signal signal_w;
    struct evloop *main_loop = ev_default_loop(0);

    ev_init(&io_w, io_action);
    ev_io_set(&io_w, STDIN_FILENO, EV_READ);

    ev_init(&timer_w, timer_action);
    ev_timer_set(&timer_w, 2, 0);//2表示2s后，0表示不是repeat的。

    ev_init(&signal_w, signal_action);
    ev_signal_set(&signal_w, SIGINT);

    ev_io_start(main_loop, &io_w);
    ev_timer_start(main_loop, &timer_w);
    ev_signal_start(main_loop, &signal_w);

    ev_run(main_loop, 0);
    mylogd("end of code");
    return 0;
}
