#include "apue.h"
#include <stdio.h>

int main(void) {
    int c;

    while((c = getc(stdin)) != EOF) {
        if (static_cast<char>(c) == 'c') {
            printf("end of input\n");
            break;
        }

        if (putc(c, stdout) == EOF) {
            err_sys("outpur error");
        }
    }

    if (ferror(stdin)) {
        err_sys("input error");
    }

    return 0;
}