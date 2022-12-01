# linux-app-component

这个仓库的目的是整理形成自己的一套linux应用框架。

目前规划了这些功能：

* 简单参数解析和保存。使用cjson作为后端。接口要简单，就一个get，一个set接口。
* 多进程通信。使用jsonrpc来作为通信机制，通过tcp连接来通信，使用自己精简的libev（只实现Linux epoll方式）作为网络通信库。jsonrpc采用同步调用的方式，如果有耗时的操作，通过线程来执行（这个基于一个event机制来封装）。

代码全部使用C语言进行实现。

因为我日常就是使用C语言，主要是嵌入式开发场景。

构建系统就使用autotools。因为使用这个后，觉得比Makefile还是要方便不少。

顺便把这个作为autotools的一个实践项目。

# 准备工作

先保证自己把cjson研究透了。

因为都依赖了这个模块，要保证安全，没有内存问题。

# 目录

```
cjson: 存放cjson的代码，编译得到libcjson.so
tinyparam: 存放参数解析和保存的目录。编译得到libtinyparam.so。
tinyipc: 存放基于jsonrpc的进程通信机制。编译得到libtinyipc.so
tinyev：基于libev进行精简实现的事件库。编译得到libtinyev.so
test: 存放测试代码
demo：存放demo代码。
```

cjson目录是不用编译的。直接把代码放进来。

先写Makefile保证这个编译出来。

编译结果都放到output目录下。

output目录

```
bin
	存放可执行文件。
lib
	存放库文件。
include
	存放头文件。
```

# autotools搭建

上面已经规划了库和目录的情况。需要生成库文件和可执行文件。

现在看看怎么用autotools来搭建编译框架。

我参考bluealsa的写法。

https://github.com/VigilantBag/bluealsa

