#include "apue.h"
#include <csignal>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#ifdef SOLARIS
#include <sys/mkdev.h>
#endif

int main(int argc, char *argv[]) {
    struct stat statbuf;
    int i;

    for (i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);
        if (stat(argv[i], &statbuf) < 0) {
            err_sys("stat error");
        }
        printf("dev = %u/%u", major(statbuf.st_dev), minor(statbuf.st_dev));
        if (S_ISBLK(statbuf.st_mode) || S_ISCHR(statbuf.st_mode)) {
            printf(" (%s) rdev = %u/%u", S_ISCHR(statbuf.st_mode) ? "character" : "block",
                   major(statbuf.st_rdev), minor(statbuf.st_rdev));
        }
        printf("\n");
    }

    exit(0);
}
