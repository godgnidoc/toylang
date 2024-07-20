<h1> ToyLang 编程语言 </h1>

Toylang 编程语言是为实践编译原理相关知识而开发的玩具编程语言，本仓库存放其 C++ 实现源码。

# 计划

本项目会随着编译原理实践视频的发布进度更新，按照计划，有如下里程碑值得关注：

1. 完成通用语法分析器生成器
2. 完成 toylang 文法设计
4. 完成 toylang 虚拟机
5. 完成 toylang 编译器

# 使用条件

本项目在下列条件下能够顺利编译运行，若您的环境条件与此不同，可能需要移植项目内容：

- OS: Linux Ubuntu 22.04.4
- Compiler: g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
- make: GNU Make 4.3
- cmake: cmake version 3.28.1

# 快速开始

1. 执行下列指令以进入开发环境。

~~~bash
#!/bin/bash

source script/setup
~~~

上述指令会添加一些可执行程序的搜索路径，并定义一些在 bash 可以直接调用的辅助函数来帮助我们提高开发效率。

2. 成功进入环境后，可以执行如下指令来编译项目

~~~bash
#!/bin/bash

build
~~~

3. 可以直接键入目标程序的名字来运行编译产物

~~~bash
#!/bin/bash 

toylang
~~~

4. 可以执行下列指令来清理编译产物和CMake缓冲

~~~bash
#!/bin/bash

clean
~~~

5. 可以执行下列指令来切换编译模式

~~~bash
#!/bin/bash

debug # 将编译选项切换为 Debug 模式
release # 将编译选项切换为 Release 模式
~~~

# 特性

> TODO