#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <errno.h>

#define MAX_FDS 1024

struct pollfd fds[MAX_FDS];

// 初始化 pollfd 数组
void poll_init() {
    for (int i = 0; i < MAX_FDS; i++) {
        fds[i].fd = -1;
    }
}

// 将一个文件描述符添加到 pollfd 数组中
void poll_add_fd(int fd, short events) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (fds[i].fd == -1) {
            fds[i].fd = fd;
            fds[i].events = events;
            fds[i].revents = 0;
            break;
        }
    }
}

// 从 pollfd 数组中删除一个文件描述符
void poll_del_fd(int fd) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (fds[i].fd == fd) {
            fds[i].fd = -1;
            break;
        }
    }
}

// 等待事件发生
int poll_wait(int timeout) {
    int ret = poll(fds, MAX_FDS, timeout);
    if (ret == -1 && errno != EINTR) {  // EINTR 表示在等待期间被信号中断
        perror("poll");
        exit(EXIT_FAILURE);
    }
    return ret;
}


// 关闭一个文件描述符
void poll_close(int fd) {
    if (close(fd) == -1) {
        perror("close");
    }
}