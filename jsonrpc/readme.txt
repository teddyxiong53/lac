jsonrpc_client.c.bak
    这个是试图用ev_io的EV_WRITE来触发write。
    但是发现不生效。
    卡住了比较久。
    现在放弃这种方式，采用pthread_cond来触发发送事件算了。
