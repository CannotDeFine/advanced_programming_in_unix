#include "apue.h"
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        err_quit("usage: a.out <pathname>");
    }

    // 按照实际用户ID和实际组ID进行访问权限测试
    if (access(argv[1], R_OK) < 0) {
        err_ret("access error for: %s", argv[1]);
    } else {
        printf("read access OK\n");
    }

    // 根据有效用户ID对访问权限进行检查
    if (open(argv[1], O_RDONLY) < 0) {
        err_ret("open error for: %s", argv);
    } else {
        printf("open for reading OK\n");
    }

    // 如果程序设置了设置用户位，那么能够在被执行时，将有效用户ID设置为该程序的所有者ID
    // 02_access_func的所有者可以改成root，那么在执行时，有效用户ID就会被设置成root
    // 从而可以打开/etc/shadow文件

    exit(0);
}