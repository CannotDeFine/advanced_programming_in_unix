#include "apue.h"
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

void set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        err_sys("fcntl F_GETFL error");
    }

    val |= flags; /* turn on */

    if (fcntl(fd, F_SETFL, val) < 0) {
        err_sys("fcntl F_SETFL error");
    }
}

void clr_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        err_sys("fcntl F_GETFL error");
    }

    val &= ~flags; /* turn on */

    if (fcntl(fd, F_SETFL, val) < 0) {
        err_sys("fcntl F_SETFL error");
    }
}

int main(void)
{
    int fd = open("./in.file", O_RDWR);
    if (fd < 0) {
        err_sys("open error");
    }

    printf("the flag value of the file: %d", fcntl(fd, F_GETFL, 0));
    
    set_fl(fd, O_APPEND);

    printf("the flag value of the file: %d", fcntl(fd, F_GETFL, 0));

    clr_fl(fd, O_APPEND);

    printf("the flag value of the file: %d", fcntl(fd, F_GETFL, 0));
    
    if (close(fd) < 0) {
        err_sys("close error");
    }

    return 0;
}