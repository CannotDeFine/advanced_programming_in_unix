#include "apue.h"
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    if (open("tempfile", O_RDWR) < 0) {
        err_sys("open error");
    }

    if (unlink("tempfile") < 0) {
        err_sys("unlink error");
    }

    printf("file unlinked\n");
    sleep(15);
    printf("done\n");
    exit(0);
}