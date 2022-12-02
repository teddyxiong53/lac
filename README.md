# linux-app-component

这个仓库的目的是整理形成自己的一套linux应用框架。

目前规划了这些功能：

* 简单参数解析和保存。使用cjson作为后端。接口要简单，就一个get，一个set接口。
* 多进程通信。使用jsonrpc来作为通信机制，通过tcp连接来通信，使用自己精简的libev（只实现Linux epoll方式）作为网络通信库。jsonrpc采用同步调用的方式，如果有耗时的操作，通过线程来执行（这个基于一个event机制来封装）。

# 动机

我写代码的基本准则是简单实用。

我一直认为：要杀一只鸡，最好的方法是买一把菜刀，而不是去建设一个屠宰场。

而在工作中，碰到很多的库，都过度设计了。阅读使用都很繁琐。出问题查起来也很麻烦。

所以我决定把一些自己常用的功能整理出来，形成自己的工具代码库。

# 语言

代码全部使用C语言进行实现。

因为我日常就是使用C语言，主要是嵌入式开发场景。

# 构建系统

构建系统就使用autotools。因为使用这个后，觉得比Makefile还是要方便不少。

顺便把这个作为autotools的一个实践项目。

## 编译方法

先使用autogen.sh生成configure文件。默认生成的那一堆文件我都加入到.gitignore不上传。

因为我看了太多的开源项目把这一堆上传了。

我最开始不了解autotools的时候，被这一堆生成的代码吓到了。其实完全没有必要上传的。

```
./autogen.sh
```

然后就生成了configure脚本（根据我们手写的configure.ac生成的）。

然后就是Linux应用源代码编译三板斧：

```
./configure
make
make install
```



# 代码风格

* 使用vscode的WebKit风格进行格式化。这个基本跟K&R C的风格是一致的，是我喜欢的排版格式。
* 所有的函数和变量都全部使用小写加下划线的方式，原则上拒绝一起驼峰。
* 所有的宏都用大写。
* 尽量不用宏展开进行函数和数据结构的实现。这样会不便于调试和阅读。

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

# 先写tinyparam

这个比较独立，也比较简单。所以从这个先写。

