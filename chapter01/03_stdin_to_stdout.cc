#include "apue.h"
#include <cstdio>

// 采用标准IO库实现系统调用

int main(void) {
    int c;

    while((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            err_sys("output error.");
        }
    }

    if (ferror(stdin)) {
        err_sys("input error.");
    }

    exit(0);
}