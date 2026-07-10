#include "apue.h"
#include <bits/types/FILE.h>
#include <sys/stat.h>

static void copy_data(FILE *srcfp, FILE *desfp);
static void copy_mode(FILE *srcfp, FILE *desfp);
static bool is_same(int srcfd, int desfd);

int main(int argc, char *argv[]) {
    
    return 0;
}

static bool is_same(int srcfd, int desfd) {
    struct stat srcbuf;
    struct stat desbuf;

    if (fstat(srcfd, &srcbuf) < 0) {
        err_quit("get the stat of source file error");
    }

    if (!S_ISREG(srcbuf.st_mode)) {
        err_quit("source file is not a regular file");
    }

    if (fstat(desfd, &desbuf) < 0) {
        err_quit("get the stat of destination file error");
    }

    if (srcbuf.st_ino == desbuf.st_ino && srcbuf.st_dev == desbuf.st_dev) {
        return true;
    }

    return false;
}