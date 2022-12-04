#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/unistd.h>
#include <ev.h>
#include "mylog.h"

ev_cleanup cleanup_w;
ev_signal signal_w;

void cleanup_action(struct ev_loop *loop, ev_cleanup *w, int e)
{

    mylogd("cleanup happen");

}
void signal_action(struct ev_loop *loop, ev_signal *signal_w, int e)
{
    mylogd("get sig:%d", signal_w->signum);
    ev_break(loop, EVBREAK_ALL);
}

int main(int argc, char const *argv[])
{
    struct ev_loop *loop = ev_default_loop(0);

    ev_cleanup_init(&cleanup_w, cleanup_action);
    ev_cleanup_start(loop, &cleanup_w);

    ev_signal_init(&signal_w, signal_action, SIGINT);
    ev_signal_start(loop, &signal_w);

    ev_run(loop, 0);

    ev_loop_destroy(loop);//必要要有这一行，才能看到cleanup的打印。

    return 0;
}
