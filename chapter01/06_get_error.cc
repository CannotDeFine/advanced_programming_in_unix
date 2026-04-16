#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc >= 2) printf("error");
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));
    errno = ENOENT;
    perror(argv[0]);
    exit(0);
}