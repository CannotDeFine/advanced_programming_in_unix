#include "apue.h"
#include <unistd.h>

#define BUFFER_SIZE 4096

// 通过read/write系统调用实现复制

int main(void) {
    int n = 0;
    char *buffer;
    buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));

    while((n = read(STDIN_FILENO, buffer, BUFFER_SIZE)) != 0) {
        if (write(STDOUT_FILENO, buffer, n) != n) err_sys("Write Error!");
    }

    if (n < 0) err_sys("Read Error!");
    exit(0);
}