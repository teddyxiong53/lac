#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/unistd.h>
#include <ev.h>
#include "mylog.h"

ev_stat stat_w;

void stat_action(struct ev_loop *loop, ev_stat *w, int e)
{
    ev_stat_stop(loop, w);
    mylogd("%s changed", w->path);
}
int main(int argc, char const *argv[])
{
    struct ev_loop *loop = ev_default_loop(0);
    ev_stat_init(&stat_w, stat_action, "./1.txt", 0);
    ev_stat_start(loop, &stat_w);
    ev_run(loop, 0);
    return 0;
}
