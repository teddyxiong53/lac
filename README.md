# linux-app-component

这个仓库的目标是整理形成自己的一套linux应用框架。

| 模块名      | 完成状态 | 说明                                                         |
| ----------- | -------- | ------------------------------------------------------------ |
| tinyparam   | √        | 描述：实现参数的读取和保存。<br/>动机：Linux下参数解析方案太多了，没有一个标准。配置语法也都各有所不同。我懒得去分析理解这些不同的配置语法了。json不就是很好的配置文件用法吗？为什么不用呢？不想引入c++的不确定性。而cjson库用起来太繁琐，随便读写一个参数得写一大串代码。<br/>基本思路：用cjson做后端，提供4个接口，一个open，一个close，一个get，一个set。用点分字符串来表示对象的层级关系。所有的值都用字符串类型表示（把参数的解释权交给上层应用）<br/>* get接口：`char* tp_get(tp_handle_t *h, char *key)`<br/>举例：`tp_get(h, "system.audio.volume")`<br/> 表示获取系统的音量值。<br/>*  set接口：`int tp_set(tp_handle_t *h, char *key, char *value);`<br/>举例：`tp_set(h, "system.audio.volume", "50")`<br/>表示设置系统音量为50。设置后参数自动保存到json文件里。 |
| jsonrpc     | √        | 描述：实现基于jsonrpc的进程通信机制。<br/>动机：dbus太难用了。基于二进制通信协议太反人类了。我只想日子好过一点。我爱json。让json来接管一切吧。jsonrpc在很多大型应用里（例如vscode）都得到广泛使用，简单实用。为什么不用？<br/>基本思路：纯粹用C语言实现，保证简单可靠，拒绝不确定性。jsonrpc默认是请求-响应模型的。但是我在进程间通信是需要server主动向client发消息的。所以我自己另外实现了一些非标准的部分，可以让server根据需要对所有的client进行广播。完美。 |
| lvgl_layout | X        | 描述：对lvgl应用使用json进行描述布局。<br/>动机：lvgl写代码还是很繁琐。代码和界面分离是一个很自然的选择。这个是目前的初步想法，也许已经有更好的做法存在了也不一定。 |
| tinythpool  | X        | 描述：一个简单的C语言线程池。<br/>动机：有些异步的操作需求。这个不难，之前做过。有时间整理优化一下。 |

更多功能，慢慢补齐，争取形成一套完整的方案。就像openwrt里的ubus、lua web那一套机制。



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

## 测试

在test目录下，有对应的测试用例。执行对应的二进制文件即可。



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

函数的错误处理：

```
我比较习惯linux kernel probe函数里的处理方式。
因为C语言没有自带的异常处理机制。
我一般是这样做的：
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
err2:
	//
err1:
	//
	return -1;
}
```





