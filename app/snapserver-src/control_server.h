#ifndef __CONTROL_SERVER_H__
#define __CONTROL_SERVER_H__
#incldue "ev.h"

/*
    control_server 在系统中扮演什么角色？
    是一个tcp server。
    其实还可以通过http来进行控制。
    但是我就不用http的了。
    接收处理控制信息。
    一个新的连接，可以理解一个session吧。
    control server的属性：
        保存所有的连接session。
        session有两种：一种是控制session，一种是stream session。
            控制session做了什么？

    方法：
        start、stop：start就是开始listen。就是一个普通的tcp server的行为。
            那能不能直接用一个json rpc server来做呢？
        send：发送消息给所有的client。

*/
struct control_server {
    ev_io watcher;

};
struct control_server * control_server_create();

#endif // __CONTROL_SERVER_H__
