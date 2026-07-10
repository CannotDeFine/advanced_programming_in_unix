#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void copy_data(int srcfd, int desfd);
static void copy_mode(int srcfd, int desfd);
static bool is_same(int srcfd, int desfd);
static void write_all(int fd, const void *buf, size_t count);

int main(int argc, char *argv[]) {
    if (argc != 3)
        err_quit("usage: mycp <source_file> <destination_file>");

    int srcfd, desfd;
    if ((srcfd = open(argv[1], O_RDONLY)) < 0) {
        err_quit("open source file error");
    }

    if ((desfd = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
        err_quit("open destination file error");
    }

    if (is_same(srcfd, desfd)) {
        close(srcfd);
        close(desfd);
        err_quit("source file and destination file is the same");
    }

    if (ftruncate(desfd, 0) < 0) {
        err_quit("truncate destination file error");
    }

    copy_data(srcfd, desfd);
    copy_mode(srcfd, desfd);

    if (close(srcfd) < 0) {
        err_quit("close source file error");
    }

    if (close(desfd) < 0) {
        err_quit("close destination file error");
    }

    return 0;
}

static void copy_data(int srcfd, int desfd) {
    char buf[1024];

    for (;;) {
        ssize_t n = read(srcfd, buf, sizeof(buf));

        if (n == 0) {
            break;
        } else if (n > 0) {
            write_all(desfd, buf, static_cast<size_t>(n));
        } else if (errno == EINTR) {
            continue;
        } else {
            err_quit("read source file error");
        }
    }
}

static void copy_mode(int srcfd, int desfd) {
    struct stat src_statbuf;
    if (fstat(srcfd, &src_statbuf) < 0) {
        err_quit("get stat of source file error");
    }

    if (fchmod(desfd, src_statbuf.st_mode & 07777) < 0) {
        err_quit("change destination file mode error");
    }
}

static bool is_same(int srcfd, int desfd) {
    struct stat src_buf;
    struct stat des_buf;

    if (fstat(srcfd, &src_buf) < 0) {
        err_quit("get stat of source file error");
    }

    if (!S_ISREG(src_buf.st_mode)) {
        err_quit("source file is not a regular file");
    }

    if (fstat(desfd, &des_buf) < 0) {
        err_quit("get stat of destination file error");
    }

    if (src_buf.st_ino == des_buf.st_ino && src_buf.st_dev == des_buf.st_dev) {
        return true;
    }

    return false;
}
 
static void write_all(int fd, const void *buf, size_t count) {
    const char *p = static_cast<const char *>(buf);

    while (count > 0) {
        ssize_t n = write(fd, p, count);

        if (n == 0) {
            err_quit("write return 0");
        } else if (n < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                err_quit("write destination error");
            }
        } else {
            p += n;
            count -= static_cast<size_t>(n);
        }
    }
}