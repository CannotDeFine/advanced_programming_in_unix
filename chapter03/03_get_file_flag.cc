#include "apue.h"
#include <cstdlib>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int val;

    if (argc != 2) {
        err_quit("usage: a.out <descriptor#>");
    }

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) == -1) {
        err_sys("fcntl error for fd %d", atoi(argv[1]));
    }

    switch (val & O_ACCMODE) {
    case O_RDONLY :
        printf("read only");
        break;
    case O_WRONLY:
        printf("write only");
        break;
    case O_RDWR:
        printf("read write");
        break;
    default:
        err_dump("unknown access mode");
    }

    if (val & O_APPEND) {
        printf(", append");
    }
    if (val & O_NONBLOCK) {
        printf(", nonblocking");
    }
    if (val & O_SYNC) {
        printf(", synchronous writes");
    }

// 如果系统定义了O_FSYNC，并且它和O_SYNC的实现不同，就单独处理
#if !defined(_POSIX_C_SOURCE) && defined (O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC) {
        printf(", synchronous writes");
    }
#endif

    putchar('\n');
    exit(0);

}