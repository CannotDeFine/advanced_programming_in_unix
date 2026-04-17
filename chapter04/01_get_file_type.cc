#include "apue.h"
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    struct stat buffer;
    const char *ptr;

    for (int i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);
        if (lstat(argv[i], &buffer) < 0) {
            err_ret("lstat error");
            continue;
        }

        if (S_ISREG(buffer.st_mode)) {
            ptr = "regular";
        } else if (S_ISDIR(buffer.st_mode)) {
            ptr = "directory";
        } else if (S_ISCHR(buffer.st_mode)) {
            ptr = "character special";
        } else if (S_ISBLK(buffer.st_mode)) {
            ptr = "block special";
        } else if (S_ISFIFO(buffer.st_mode)) {
            ptr = "fifo";
        } else if (S_ISLNK(buffer.st_mode)) {
            ptr = "symbolic link";
        } else if (S_ISSOCK(buffer.st_mode)) {
            ptr = "socket";
        } else {
            ptr = "** unknown mode **";
        }
        printf("%s\n", ptr);
    }
    exit(0);
}
