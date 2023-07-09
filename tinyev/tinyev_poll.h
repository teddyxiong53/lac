#ifndef __TINYEV_POLL_H__
#define __TINYEV_POLL_H__

void poll_init() ;
void poll_add_fd(int fd, short events);
void poll_del_fd(int fd);

int poll_wait(int timeout) ;

void poll_close(int fd);
#endif // __TINYEV_POLL_H__
