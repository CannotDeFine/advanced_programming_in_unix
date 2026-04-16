# advanced_programming_in_unix

这份笔记围绕 UNIX/Linux 下常见的系统调用与相关库函数展开，结合当前仓库 `chapter01` 的示例程序整理。重点不是死记函数原型，而是理解它们在进程、文件、目录、标准 IO、错误处理和信号机制中的角色。

## 1. UNIX 编程的基本认识

UNIX 编程里经常会同时接触两类接口：

- 系统调用：由内核提供，例如 `read`、`write`、`fork`、`exec`、`waitpid`。
- 库函数：由 C 标准库或系统库提供，例如 `printf`、`fgets`、`strerror`、`perror`、`opendir`。

两者的关系可以简单理解为：

- 系统调用更靠近内核，通常直接操作文件描述符、进程、信号等内核对象。
- 库函数更偏向“易用性封装”，有些最终仍会落到系统调用上。

例如：

- `read`/`write` 直接对文件描述符操作。
- `getc`/`putc` 属于标准 IO，带用户态缓冲。

## 2. 文件描述符与标准输入输出

UNIX 中一切皆文件，进程默认会打开三个标准文件描述符：

- `STDIN_FILENO`：标准输入，值通常是 `0`
- `STDOUT_FILENO`：标准输出，值通常是 `1`
- `STDERR_FILENO`：标准错误，值通常是 `2`

在 [chapter01/02_stdin_to_stdout.cc](/home/cdf/advanced_programming_in_unix/chapter01/02_stdin_to_stdout.cc:1) 中，程序使用 `read` 和 `write` 做了一个最小复制程序：

- `read(fd, buf, n)`：从文件描述符 `fd` 读取最多 `n` 字节，返回实际读取字节数。
- `write(fd, buf, n)`：向文件描述符 `fd` 写入 `n` 字节，返回实际写入字节数。

使用时要点：

- `read` 返回 `0` 表示读到 EOF。
- `read` 返回 `-1` 表示出错，需要检查 `errno`。
- `write` 可能发生“短写”，稳妥写法应循环写完。

当前示例已经足够说明最基本的数据流方向：

`stdin -> read -> buffer -> write -> stdout`

## 3. 标准 IO 与无缓冲 IO

在 [chapter01/03_stdin_to_stdout.cc](/home/cdf/advanced_programming_in_unix/chapter01/03_stdin_to_stdout.cc:1) 中，程序改用标准 IO：

- `getc(stdin)`：从 `stdin` 读取一个字符。
- `putc(c, stdout)`：向 `stdout` 输出一个字符。
- `ferror(stdin)`：检查流是否发生错误。

和 `read`/`write` 相比，标准 IO 的特点是：

- 更方便，接口更高层。
- 默认带缓冲。
- 操作对象是 `FILE *`，而不是文件描述符。

一般可以这样理解：

- 需要精确控制底层行为时，优先考虑 `read`/`write`。
- 需要方便地按字符、按行、格式化处理时，标准 IO 更顺手。

## 4. 目录操作

在 [chapter01/01_ls.cc](/home/cdf/advanced_programming_in_unix/chapter01/01_ls.cc:1) 中出现了目录相关函数：

- `opendir(path)`：打开目录，返回 `DIR *`。
- `readdir(dirp)`：读取目录项，返回 `struct dirent *`。
- `closedir(dirp)`：关闭目录流。

这一组接口说明：

- 目录不是普通文本文件，通常要通过专门的目录流接口访问。
- `readdir` 每次返回一个目录项，读完后返回 `NULL`。

常见用途：

- 实现简化版 `ls`
- 遍历目录树
- 收集文件名列表

## 5. 进程标识

在 [chapter01/04_getpid.cc](/home/cdf/advanced_programming_in_unix/chapter01/04_getpid.cc:1) 中使用了：

- `getpid()`：获取当前进程的进程 ID。

理解它的意义很重要：

- 每个进程都有唯一的 PID。
- 父子进程、信号发送、进程等待等机制都依赖 PID。

以后你还会经常见到：

- `getppid()`：获取父进程 PID。
- `getuid()` / `geteuid()`：获取用户身份。
- `getgid()` / `getegid()`：获取组身份。

## 6. 进程创建与程序替换

在 [chapter01/05_get_command_and_exec.cc](/home/cdf/advanced_programming_in_unix/chapter01/05_get_command_and_exec.cc:1) 中，已经出现了 UNIX 进程控制最核心的一条链路：

`fork -> exec -> waitpid`

### `fork`

- `fork()`：创建子进程。
- 返回值规则：
- 在父进程中返回子进程 PID。
- 在子进程中返回 `0`。
- 返回 `< 0` 表示创建失败。

`fork` 后会产生两个几乎一样的执行流，因此代码通常写成：

```c
if ((pid = fork()) < 0) {
    // error
} else if (pid == 0) {
    // child
} else {
    // parent
}
```

### `exec`

示例中使用：

- `execlp(buffer, buffer, (char*)0)`

它的作用不是“创建新进程”，而是：

- 用一个新程序替换当前进程映像。

所以：

- `fork` 负责“生出一个子进程”。
- `exec` 负责“让这个子进程去执行别的程序”。

`execlp` 中的 `p` 表示会按 `PATH` 环境变量查找可执行文件。

### `waitpid`

- `waitpid(pid, &status, 0)`：等待指定子进程结束。

它的作用：

- 防止子进程结束后变成僵尸进程。
- 让父进程能够拿到子进程的终止状态。

这是 shell、守护进程、任务管理器等程序的基础能力。

## 7. 按行读取输入

在 `05_get_command_and_exec.cc` 和 `08_signal.cc` 中都使用了：

- `fgets(buffer, MAXLINE, stdin)`

特点：

- 读取一行，包括结尾换行符。
- 成功时返回缓冲区指针。
- 到达 EOF 或出错时返回 `NULL`。

示例中又配合了：

- `strlen`

用于把结尾的 `\n` 改成字符串结束符 `\0`，方便后续当作命令名传给 `execlp`。

## 8. 错误处理

在 [chapter01/06_get_error.cc](/home/cdf/advanced_programming_in_unix/chapter01/06_get_error.cc:1) 中，核心是三类错误处理方式：

- `strerror(errnum)`：把错误号转换为可读字符串。
- `perror(prefix)`：输出 `prefix: 错误描述`。
- `errno`：保存最近一次出错原因。

典型理解方式：

- 失败的系统调用或库函数通常通过返回值告诉你“失败了”。
- 具体为什么失败，再通过 `errno` 追查。

例如：

- `EACCES`：权限不够
- `ENOENT`：文件或路径不存在

APUE 里常见的 `err_sys`、`err_ret` 是作者提供的辅助封装，本质上还是基于 `errno` 做格式化输出。

## 9. 用户 ID 与组 ID

在 [chapter01/07_uid_and_gid.cc](/home/cdf/advanced_programming_in_unix/chapter01/07_uid_and_gid.cc:1) 中出现：

- `getuid()`：获取实际用户 ID
- `getgid()`：获取实际组 ID

这两个值决定了进程默认的访问身份。它们直接影响：

- 能否读取某个文件
- 能否写入某个目录
- 能否向其他进程发送某些信号

需要区分的概念：

- 实际用户 ID：谁启动了这个进程
- 有效用户 ID：当前以内核看来，进程在以谁的身份做权限检查

后面学 `setuid` 程序时，这个区别会非常关键。

## 10. 信号机制

在 [chapter01/08_signal.cc](/home/cdf/advanced_programming_in_unix/chapter01/08_signal.cc:1) 中出现了：

- `signal(SIGINT, sig_int)`

它表示为 `SIGINT` 安装一个信号处理函数。常见场景就是用户按下 `Ctrl-C`。

示例程序中的逻辑是：

- 正常打印提示符 `% `
- 用户按 `Ctrl-C`
- 进程收到 `SIGINT`
- 进入 `sig_int` 处理函数
- 输出 `interrupt`

这能帮助理解一件很重要的事：

- 信号是异步事件。
- 它会打断进程当前的正常执行流。

### 信号处理函数里的注意事项

信号处理函数里不要随意调用所有库函数。更稳妥的原则是：

- 只调用 async-signal-safe 的函数。

例如：

- `write` 通常可用于信号处理函数。
- `printf` 通常不适合直接在信号处理函数中调用。

这也是为什么 `08_signal` 里更合理的写法是使用：

- `write(STDOUT_FILENO, msg, sizeof(msg) - 1)`

## 11. `exit`

多个示例最后都调用了：

- `exit(0)`

作用：

- 正常终止进程。
- 执行标准 IO 刷新、`atexit` 注册函数等清理动作。

一般约定：

- `0` 表示成功退出。
- 非 `0` 表示异常或错误退出。

子进程在 `exec` 失败后常见写法是：

- `exit(127)`

这是 shell 世界里较常见的一个失败返回码。

## 12. 当前仓库第一章涉及到的函数速查

### 目录与文件

- `opendir`：打开目录
- `readdir`：读取目录项
- `closedir`：关闭目录
- `read`：从文件描述符读取
- `write`：向文件描述符写入

### 标准 IO

- `getc`：读一个字符
- `putc`：写一个字符
- `fgets`：读一行
- `printf`：格式化输出
- `fprintf`：向指定流格式化输出
- `perror`：结合 `errno` 输出错误
- `ferror`：检查流错误

### 字符串与错误

- `strlen`：字符串长度
- `strerror`：错误号转文本

### 进程控制

- `getpid`：取当前进程 PID
- `fork`：创建子进程
- `execlp`：按 `PATH` 查找并执行程序
- `waitpid`：等待子进程
- `exit`：结束进程

### 身份与权限

- `getuid`：取实际用户 ID
- `getgid`：取实际组 ID

### 信号

- `signal`：安装信号处理函数

## 13. 学习这些函数时建议关注的四个问题

以后每学一个新函数，建议固定问自己四件事：

1. 它操作的对象是什么。
2. 成功时返回什么，失败时返回什么。
3. 出错后应该看 `errno` 还是别的状态。
4. 它和哪些函数通常成对或成链出现。

例如：

- `fork` 常和 `exec`、`waitpid` 连用。
- `opendir` 常和 `readdir`、`closedir` 连用。
- `read` 常和 `write` 连用。

## 14. 后续可以继续补的方向

这份笔记目前主要覆盖第一章。后续你可以继续按专题补充：

- 文件 IO：`open`、`close`、`lseek`
- 文件属性：`stat`、`fstat`、`chmod`、`chown`
- 进程关系：`exec` 家族、僵尸进程、孤儿进程
- 信号进阶：`sigaction`、信号屏蔽、可重入函数
- 进程间通信：管道、FIFO、消息队列、共享内存
- 线程：`pthread_create`、互斥锁、条件变量

如果你愿意，我下一步可以继续帮你把这份笔记拆成“按函数分类”的版本，或者直接给你补一份“chapter01 每个示例对应知识点”的精简版速记。
