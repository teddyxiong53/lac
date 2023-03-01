# 目录说明

| 目录/文件             | 说明                                                         |
| --------------------- | ------------------------------------------------------------ |
| pikascript-api        | 这个目录是生成的代码。                                       |
| pikascript-core       | 这个pikapython的核心代码，是自动下载的。执行pikaPackage命令会自动下载。下载的依据是requestment.txt里罗列的内容。 |
| pikascript-lib        | 跟pikascript-core的类似，都是自动下载的。                    |
| rust-msc-latest-linux | 预编译工具，是把main.py等代码编译成c代码                     |
| pikaPackage           | 包管理工具。执行时自动根据requestment.txt的内容下载。        |

# 使用方法

先下载需要的包，修改requestment.txt内容，加入想要的package及版本号（最新版本就写==latest）

```
./pikaPackage
```

进行预编译

```
./rust-msc-latest-linux
```

