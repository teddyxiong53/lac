#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/unistd.h>
#include <ev.h>
#include "mylog.h"

ev_periodic periodic_w;

void periodic_action(struct ev_loop *loop, ev_periodic *w, int e)
{

    mylogd("periodic happen");

}

static ev_tstamp custom_periodic_cb(ev_periodic *w, ev_tstamp now)
{
    mylogd("custom periodic time");
    return now+3;
}
int main(int argc, char const *argv[])
{
    struct ev_loop *loop = ev_default_loop(0);
    // 第一种方式
    // ev_periodic_init(&periodic_w, periodic_action, 0, 3, 0);
    // ev_periodic_start(loop, &periodic_w);
    // ev_run(loop, 0);

    //第二种方式，就是自定义callback。
    ev_periodic_init(&periodic_w, periodic_action, 0, 0, custom_periodic_cb);
    ev_periodic_start(loop, &periodic_w);
    ev_run(loop, 0);


    return 0;
}
