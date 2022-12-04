#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/unistd.h>
#include <ev.h>
#include "mylog.h"

ev_prepare prepare_w;
ev_check check_w;
ev_periodic periodic_w;
ev_idle idle_w;


void prepare_action(struct ev_loop *loop, ev_prepare *w, int e)
{
    mylogd("prepare ");
}

void check_action(struct ev_loop *loop, ev_check *w, int e)
{
    mylogd("check");
}
void periodic_action(struct ev_loop *loop, ev_periodic *w, int e)
{
    mylogd("periodic");
}

void idle_action(struct ev_loop *loop, ev_idle *w, int e)
{
    // 一般在idle这里做一些后台备份之类的耗时操作。
    mylogd("idle");
}

int main(int argc, char const *argv[])
{
    struct ev_loop *loop = ev_default_loop(0);

    ev_prepare_init(&prepare_w, prepare_action);
    ev_prepare_start(loop, &prepare_w);

    ev_check_init(&check_w, check_action);
    ev_check_start(loop, &check_w);

    ev_idle_init(&idle_w, idle_action);
    ev_idle_start(loop, &idle_w);

    ev_periodic_init(&periodic_w, periodic_action, 0, 3, 0);
    ev_periodic_start(loop, &periodic_w);


    ev_run(loop, 0);

    ev_loop_destroy(loop);

    return 0;
}
