#include "apue.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

int main(void) {
    if (chdir("/tmp") < 0) {
        err_sys("change work directory error");
    }

    printf("chdir to /tmp successed\n");
    exit(0);
}