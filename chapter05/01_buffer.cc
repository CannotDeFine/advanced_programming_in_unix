#include "apue.h"
#include <cstdio>

int main() {
    printf("Hello world!");             // no newline is printed, so the line-buffered output is not flushed until 5 seconds later.
    //printf("Hello world!\n");                // compare this with the previous printf call.
    sleep(5);

    printf("Hello world again\n");      // the newline character causes the output to be flushed to the terminal.
    return 0;
}