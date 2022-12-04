#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/unistd.h>
#include <ev.h>
#include "mylog.h"

ev_child child_w;

void child_action(struct ev_loop *loop, ev_child *w, int e)
{
    ev_child_stop(loop, w);
    mylogd("child process %d exit status:%d", w->rpid, w->rstatus);
}
int main(int argc, char const *argv[])
{
    struct ev_loop *loop = ev_default_loop(0);
    pid_t pid = fork();
    if (pid < 0) {
        myloge("fork fail");
        return -1;
    } else if (pid == 0) {
        mylogd("child process here ");
        //子进程直接退出
        exit(0);
    } else {
        mylogd("child process id:%d", pid);
        //父进程里，监听子进程的状态。
        ev_child_init(&child_w, child_action, pid, 0);
        ev_child_start(loop, &child_w);
        ev_run(loop, 0);
    }
    return 0;
}
