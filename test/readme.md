| 文件名                     | 说明                                                         |
| -------------------------- | ------------------------------------------------------------ |
| test-dl-list.c             | 测试dl_list双向链表                                          |
| test-event-handler.c       | 测试event-handler模块。异步事件处理。                        |
| test-jsonrpc-client.c      | 测试jsonrpc client。需要跟test-jsonrpc-server.c配合测试。    |
| test-jsonrpc-server.c      | 这个可以单独测试。另外用nc localhost 2345连接然后发送json命令即可。 |
| test-libev-3-basic-event.c | 测试libev的3个基本事件：io、timer、signal。                  |
| test-libev-async.c         | 测试libev的ev_async事件。                                    |
| test-libev-child.c         | 测试libev的子进程事件。                                      |
| test-libev-cleanup.c       | 测试libev的清理事件。                                        |
| test-libev-periodic.c      | 测试libev的周期性事件。                                      |
| test-libev-prepare-check.c | 测试libev的prepare和check回调。                              |
| test-libev-stat.c          | 测试libev的stat事件。                                        |
| test-libev.c               | 测试libev基本功能。                                          |
| test-libevent-hello.c      | 测试libevent的基本功能，实现了echo服务器。                   |
| test-lua.c                 | 测试lua。                                                    |
| test-thpool.c              | 测试线程池。                                                 |
| test-tinyparam.c           | 测试tinyparam功能。                                          |