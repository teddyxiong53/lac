# 命名由来

LAC是`linux-app-component`的首字母缩写。发音跟英文单词`luck`相同。

这个项目的目标是整理形成自己的一套Linux应用框架，并基于这个框架实现一些有意思的个人应用。

# 项目动机

我只在Linux上开发，主要是不带界面的应用。所以不考虑兼容windows、BSD、macos。所以在C库上我优先选择musl而不是glibc。musl C库的代码干净整洁易于阅读和理解。

我喜欢干净整洁的代码。

我喜欢蛇形命名风格，至少在C语言里不愿意看到驼峰的风格。驼峰风格总让人需要去权衡一些缩写是否需要全部大写。例如IP是写成Ip还是IP、TCP是写成Tcp还是TCP、HTTP是写成Http还是HTTP、AVRCP是写成Avrcp还是AVRCP。有选择就会有分歧。这样太心累了。直接全部小写加下划线，世界清静了。

我喜欢C语言，喜欢Python和lua，所以会集成micropython和lua。不喜欢C++，所以不会有一行C++代码出现在本项目里（也因为我hold不住C++，我认为对C++投入时间精力是毫无价值的）。喜欢一切小巧的常用库实现。

不喜欢C语言大量使用宏进行代码生成（这个主要是C语言模板功能的缺失，只能用宏来模拟跟模板类似的功能）。这样让代码可读性（你可能根本没法直接在源代码里看到一个完整的函数名的存在，都是编译时动态拼接起来的函数名）和可调试性大大降低。

不喜欢因为版本问题去查一些乱七八糟的问题。所以所有的依赖的库选一个较新的稳定版本集成进来，当然是代码量很小的库。

我的项目是新的，没有历史包袱，加上这个项目的主要还是研究目的（顺带可以外溢一部分成果应用到工作中），所以我在选择依赖库的时候，就比较轻松，选择自己喜欢的就好，最好是新的不带历史包袱的库。

基于上面的想法，我期望中的这个项目可以达成这样的最终效果：

```
Linux(系统) + musl(C库) + toybox(基本命令行工具) + lac(就是本项目啦) = 一个可用的系统
```

从上面也可以看到LAC的定位就是：一个Linux应用开发框架（这个想法有点大，但是有价值的目标才会产生足够的动力）。

为什么选择toybox？其实也就是在busybox和toybox之间二选一。toybox代码可读性更好，更简单，所以我选择toybox。（虽然toybox比busybox更弱一些，但是我不care）

一切数据交换的基本格式就是json。应用的参数配置文件也全部使用json。

io多路复用机制选择libev。可选的有libevent、libev、libuv、libtuv。为什么选择libev？因为它的代码量最小，性能也不错。虽然使用了我比较讨厌的很多宏技巧，但是好在对外提供的接口比较简洁直观，所以我可以容忍它很多的宏。

所有进程间通信首选机制是在TCP链路基础上的jsonrpc。

为什么是TCP？这个选择受陈硕的影响。TCP稳定可靠，可以通过抓包工具来抓包调试问题，如果有需要，可以很容易都单机扩展到多机。

为什么是jsonrpc？因为简单够用了，而且是基于json的。自己在这个基础上增加server主动向client发送消息的机制。这样就可以替代dbus。



# 项目目录基本结构

```
app/
	这个目录下存放基于本框架编写的应用。
	目前实现了几个基本的demo：
	bgservice：作为jsonrpc server，提供服务。
	uiapp：作为jsonrpc client界面应用。
	cliapp：作为jsonrpc client的命令行应用。可用于调试目的。
	规划中的应用：
	mini-avs：基于本框架实现avs-sdk。本来的avs-sdk是用C++11写的，感觉过度设计了。搞得太复杂。
	snapcast：包括server和client。这个项目是我接触jsonrpc的第一个项目。觉得不错。但是是基于c++的。
		而且是基于boost库的。我喜欢项目本身，但是不喜欢它用的语言和库。
		必须给它改造成C语言的版本。并且以这个app的需要来驱动我补全必要的模块支持。
test/
	这个目录下是各个模块的测试代码。
	所有命名都是test-xx.c的格式。每个test-xx.c都是一个单独的程序。
	没有使用什么unittest框架。目前来说够用。
其余目录：
	一个目录是一个模块。大多数会编译出一个动态库文件。
	有的可能会是编译出可执行文件。
	
```

# 模块规划

模块可以划分为这几个大类：

* 基础库
* 网络库
* 多媒体库
* 脚本语言库

完成状态有：√（完成）、x（还没有写）、~（进行中）。

| 模块名          | 完成状态 | 说明                                                         |
| --------------- | -------- | ------------------------------------------------------------ |
| 基础库          |          |                                                              |
| tinyparam       | √        | 描述：实现参数的读取和保存。<br/>动机：Linux下参数解析方案太多了，没有一个标准。配置语法也都各有所不同。我懒得去分析理解这些不同的配置语法了。json不就是很好的配置文件用法吗？为什么不用呢？不想引入c++的不确定性。而cjson库用起来太繁琐，随便读写一个参数得写一大串代码。<br/>基本思路：用cjson做后端，提供4个接口，一个open，一个close，一个get，一个set。用点分字符串来表示对象的层级关系。所有的值都用字符串类型表示（把参数的解释权交给上层应用）<br/>* get接口：`char* tp_get(tp_handle_t *h, char *key)`<br/>举例：`tp_get(h, "system.audio.volume")`<br/> 表示获取系统的音量值。<br/>*  set接口：`int tp_set(tp_handle_t *h, char *key, char *value);`<br/>举例：`tp_set(h, "system.audio.volume", "50")`<br/>表示设置系统音量为50。设置后参数自动保存到json文件里。 |
| jsonrpc         | √        | 描述：实现基于jsonrpc的进程通信机制。<br/>动机：dbus太难用了。基于二进制通信协议太反人类了。我只想日子好过一点。我爱json。让json来接管一切吧。jsonrpc在很多大型应用里（例如vscode）都得到广泛使用，简单实用。为什么不用？<br/>基本思路：纯粹用C语言实现，保证简单可靠，拒绝不确定性。jsonrpc默认是请求-响应模型的。但是我在进程间通信是需要server主动向client发消息的。所以我自己另外实现了一些非标准的部分，可以让server根据需要对所有的client进行广播。 |
| tinythpool      | √        | 描述：一个简单的C语言线程池。<br/>动机：有些异步的操作需求。 |
| event_handler   | ~        | 描述：可以给各个模块单独使用的异步事件处理。跟tinythpool的区别是，这个是各个模块专用的。可以方便各个模块自己控制。<br/>基本功能有了。代码还需要完善。 |
| libev           | √        | 描述：这个就是libev的代码，我只选择了其中epoll的实现（poll和select都不要）。config.h也自己手动修改的。 |
| tinyev          | ~        | 描述：这个是对libev进行实现，我不喜欢libev大量使用宏的方式，所以自己对libev进行简化，但是这个一时半会儿写不完，而且libev是我的系统里重要的基础库，所以也不想冒险去替换，以免带来各种奇怪难查的问题。 |
| tinyutils       | x        | 描述：一些基础的工具函数，例如file_util、str_util等等。有需要的时候再一个个加进来。 |
| tinyds          | x        | 描述：ds表示data structure，这个目录放一些基础的通用数据结构。例如双向链表。 |
| include/mylog.h | √        | 描述：一个简单到不能再简单的log函数。只有十几行代码，一秒钟就能看懂实现。提供了mylogd/mylogi/mylogw/myloge这4个接口。分别对应debug、info、warning、error 4个级别的打印。<br/>打印举例：<br/>`[DEBUG][jsonrpc_server.c][connection_cb][197]: read len:26`<br/>还是我的一贯原则：简单实用。 |
| 网络库          |          |                                                              |
| http_server     | x        | 描述：实现http server的功能。基于libev。                     |
| http_client     | x        | 描述：实现一个http client的功能。libcurl感觉有些复杂。看看有没有更简单的 |
| mqtt_client     | ~        | 描述：一个mqtt客户端，实现连接broker进行pub和sub操作。       |
| ws_client       | x        | 描述：一个websocket client端。                               |
| 多媒体库        |          |                                                              |
| audioplayer     | x        | 描述：实现音频文件的播放。用C语言实现面向对象的方式。目前规划子类：wavplayer和mp3player。 |
| tinyalsa        | x        | 描述：其实就是google的tinyalsa。但是我会自己实现一遍。保持对外接口不变。 |
| streamplayer    | x        | 描述：这个是在线的音频播放。跟audioplayer不一样。之前只用ffmpeg做过。但是现在不想引入ffmpeg。看看有没有其他的方式可以实现。 |
| 脚本语言库      |          |                                                              |
| micropython     | x        | 描述：就是官方的micropython仓库，相比于python，这个的大小更加可控。我会只保留代码里的Linux需要的部分，其余代码一律删除。 |
| lua             | x        | 描述：就是lua官方仓库，因为这个官方版本就足够精简了。这个全部集成进来，自己写编译代码。 |

更多功能，慢慢补齐，争取形成一套完整的方案。就像openwrt里的ubus、lua web那一套机制。

更多功能，根据后续的需要慢慢补齐，争取形成一套完整的方案。

# 构建系统

构建系统就使用autotools。

这个是一个三选一的题目：

* make简单，但是写起来起来麻烦。
* cmake强大，还是写起来麻烦。
* autotools，强大，没那么麻烦。

我最怕麻烦，所以就是autools了。



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

## 测试

在test目录下，有对应的测试用例。执行对应的可执行文件即可。

## 调试方法

可以用gdb和valgrind进行调试。

在使用gdb和valgrind之前，需要搞清楚一点：

test目录下生成的文件，例如test-jsonrpc-server，实际上是libtools生成的包装脚本。无法对这个脚本文件进行gdb调试。真正的二进制文件是在test/.libs/test-jsonrpc-server。

如果要用gdb进行调试，需要先把需要的动态库加入到LD_LIBRARY_PATH里。

例如：

```
export LD_LIBRARY_PATH=~/work/test/lac/jsonrpc/.libs:\
~/work/test/lac/cjson/.libs:\
~/work/test/lac/libev/.libs:\
~/work/test/lac/tinyparam/.libs
```

然后在用gdb进行调试：

```
gdb test/.libs/test-jsonrpc-server
```

valgrind的情况是一样的。



# 代码风格

* 使用vscode的WebKit风格进行格式化。这个基本跟K&R C的风格是一致的，是我喜欢的排版格式。
* 所有的函数和变量都全部使用小写加下划线的方式，原则上拒绝一切驼峰。
* 所有的宏都用大写。
* 尽量不用宏展开进行函数和数据结构的实现。这样会不便于调试和阅读。

# 函数命名语义

总体来说，我的代码遵循这样的规则：

```
struct xx * xx_create()
	create语义表示内部需要分配一个xx结构体。
	逻辑上，跟 void xx_destroy(struct xx *x) 进行配对使用。
int xx_init(struct xx *x)
	init表示对一个已经存在的对象进行初始化，传递的参数x指针，可能是一个静态的变量。也可以是malloc的内存。
	逻辑上没有配对的函数。
	
```

有些从其他地方拿过来的代码不一定符合这个规则。

如果有需要注意的地方，我会在头文件注释里进行说明。

# 函数错误处理

我比较习惯linux kernel probe函数里的处理方式。因为C语言没有自带的异常处理机制。我一般是这样做的：

```c
int xx_init(struct xx *x) 
{
	if (err1) {
		//log
		goto err1;
	}
	if (err2) {
		//log
		goto err2;
	}
	return 0;
err2:
	//
err1:
	//
	return -1;
}
```

# 关于返回值

一般一套框架库，都会自己定义一套返回值。

但是我觉得没必要。

所以我的函数返回值很简单：

```
失败返回-1
正常返回0
有错误通过myloge打印出来。
```

还是那句话，够用了。

我实在不想去记那么多返回值，不想去纠结某个错误到底应该返回哪个错误码。

## 布尔语义

但是避免不了有些情况需要布尔语义，例如这个函数：

```c
int file_util_exists(char *name)
```

这个函数应该返回什么返回值呢？

如果一律按照正常返回0，失败返回-1。

那么调用时就应该这样：

```c
if (!file_uitl_exists("1.txt")) {//文件存在
	mylogd("1.txt exists");
}
```

这个写法从人的直觉上，是有些别扭的。

因为`!file_uitl_exists`从直觉上，是说这个文件不存在。

所以布尔布尔语义是有非常有必要引入的。

为了使用布尔语义，需要引入：

```c
#include <stdbool.h>
```

使用bool类型的返回值：

```c
bool file_util_exists(char *name)
```

这样使用起来就符合直觉了：

```c
if (!file_util_exists("1.txt")) {
	mylogd("file not exists");
} else {
	mylogd("file exists");
}
```

把需要布尔语义的函数返回值，跟普通的int返回值区分开来。



