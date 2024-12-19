<h1>0序章</h1>

> Version 1.0.1 by GodGnidoc; Jul. 11, 2024

# 1. 引言

现代程序设计语言的编译器通常由前端和后端两部分构成。

前端被用于处理语言自身的细节，包括词法分析，语法分析和语义分析。

而后端主要用于处理关于目标系统的细节以生成目标代码，包括寄存器选择、表达式优化等。

出于对计算机技术的热爱或好奇，本系列视频将带领大家从零开发属于我们自己的通用语法分析器。

完成全部章节后，我们将得到一个高度可扩展的工具。它以目标语言的属性文法和源代码作为输入。生成语义分析树。

定位上，我们的目标工具应当类比于 GNU/flex、GNU/bison 或 ANTLR 工具。只是功能的广度和深度都必然无法和上述工具相提并论。如果本系列视频能够得到大家的认可，我将尝试推出后续系列视频，跟大家一起学习更多编译技术直到完成一款功能完备的现代高级编程语言为止。

# 2. 系列结构

本系列视频由如下章节构成：

首先，第一章主要讨论词法分析技术，包括正则表达式的分析，确定有穷状态机的实现以及状态跳转图的生成等方面。

接下来第二章主要讨论SLR语法分析技术。包括 NULLABLE、FIRST、FOLLOW 集的计算、项集族的生成等方面。

在第三章文法设计中，我们将逐步设计出用于定义新语言的文法格式，并实现文法文件的读取和分析。

第四章讨论如何在产生式上添加语义属性以形成简单的属性文法，并改造语法分析器，在语法分析的同时收集和绑定语义属性，构建语义分析树。

在第五章我们将把上述章节所讨论的全部细节集成起来，并解决一些诸如日志打印，资源管理等工程问题，完成我们通用语法分析器工具的开发。

# 3. 风格介绍

本系列视频不是教程，其中所提及和使用的技术细节可能存在欠缺或谬误，欢迎指正。

风格方面，本系列视频倾向于使用与文案紧密搭配的图示或动画来降低观众的理解和记忆成本。

每一章节的目标都有配套的编码实践环节，相应的编码成果也会同步在 github 上开源。

# 4. 条件准备

为了完成本系列视频的全部章节，您需要准备如下三个方面的条件：

硬件方面：

1. 首先，您需要一张有腰部支撑的椅子，它会在接下来的旅途中保护我们的腰椎。
2. 此外，我们还需要一瓶眼药水，它能让代码看起来光鲜靓丽不再苦涩酸痛。
3. 最后，如果您不顾家人朋友的劝阻，真心想写代码的话。您将需要一台**年轻**的电脑和它的充电器。

接下来，软件方面：

1. 您的电脑需要搭载Linux 或 Windows 操作系统。
   其它操作系统笔者尚未接触过，要成功运行接下来章节中的代码可能需要适当移植。
   在本系列视频制作期间，笔者正在使用 Ubuntu 22.04.4 操作系统。这是一款很受欢迎的桌面端 Linux 操作系统
   如果有小伙伴希望降低试错成本，可以选择同款的操作系统。
2. 为了能从我们的代码生成可用的工具，我们还需要一款能与 C++17 和平相处的编译器。
   笔者的编译器为 g++ 11.4.0。同样，为了降低试错成本，可以考虑使用与笔者同款的编译器。
3. 最后，完成本系列视频的必备条件：一个炫酷且流畅的代码编辑器
   笔者的选择是伟大的 VisualStudio...Code！
   老规矩，如果小伙伴们想要降低试错成本，可以考虑选择同款编辑器。

在技能方面：

1. 小伙伴们只需要简单的 C++ 语言编程能力即可。

# 5. 总结

以上便是序章的全部内容。

下期视频我们计划开始第一章词法分析的第一节，关于确定有穷状态机和正则表达式的讨论。

当然，如果有需要的话，我会在第一章之前插入一章内容，手把手带大家准备一套同款环境。

祝大家生活愉快，我们下期视频再见。