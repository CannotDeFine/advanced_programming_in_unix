// 使用O_TRUNC选项将文件长度截断为0，但是不改变访问时间和修改时间

#include "apue.h"
#include <bits/types/struct_timespec.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int i, fd;
    struct stat statbuf;
    struct timespec times[2];

    for (i = 1; i < argc; i++) {
        if (stat(argv[i], &statbuf) < 0) {
            err_ret("%s: stat error", argv[i]);
            continue;
        }

        if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
            err_ret("%s: open error", argv[i]);
            continue;
        }

        times[0] = statbuf.st_atim;
        times[1] = statbuf.st_mtim;
        if (futimens(fd, times) < 0) {
            err_ret("%s: futimens error", argv[i]);
        }

        close(fd);
    }

    return 0;
}