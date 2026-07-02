#include "apue.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <linux/limits.h>
#include <sys/stat.h>

/* function type that is called for each filename */
typedef int Myfunc(const char *, struct stat *, int);

static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot; /* file type */

int main(int argc, char *argv[]) {
    int ret;
    if (argc != 2)
        err_quit("usage: ftw <starting-pathname>");
    ret = myftw(argv[1], myfunc);

    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    if (ntot == 0)
        ntot = 1;
    printf("%-16s = %7ld, %6.2f%%\n", "regular files", nreg, nreg * 100.0 / ntot);
    printf("%-16s = %7ld, %6.2f%%\n", "directories", ndir, ndir * 100.0 / ntot);
    printf("%-16s = %7ld, %6.2f%%\n", "block special", nblk, nblk * 100.0 / ntot);
    printf("%-16s = %7ld, %6.2f%%\n", "char special", nchr, nchr * 100.0 / ntot);
    printf("%-16s = %7ld, %6.2f%%\n", "FIFOs", nfifo, nfifo * 100.0 / ntot);
    printf("%-16s = %7ld, %6.2f%%\n", "symbolic links", nslink, nslink * 100.0 / ntot);
    printf("%-16s = %7ld, %6.2f%%\n", "sockets", nsock, nsock * 100.0 / ntot);

    exit(ret);
}

/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */

#define FTW_F 1   /* file other than directory */
#define FTW_D 2   /* directory */
#define FTW_DNR 3 /* directory thar can't be read */
#define FTW_NS 4  /* file that we can't stat */

static char *fullpath; /* contains full pathname for every file */
static size_t pathlen;

static int myftw(char *pathname, Myfunc *func) {
    fullpath = path_alloc(&pathlen); /* malloc PATH_MAX + 1 bytes */
    if (pathlen <= strlen(pathname)) {
        pathlen = strlen(pathname) * 2;
        if ((fullpath = static_cast<char *>(realloc(fullpath, pathlen))) == nullptr) {
            err_sys("realloc error");
        }
    }

    strcpy(fullpath, pathname);
    return (dopath(func));
}

/*
 *Descend through the hierarchy, starting at "fullpath"
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return. For a directory, we call ourself
 * recursively for each name in the directory.
 */

static int dopath(Myfunc *func) { /* we return whatever func() returns */
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;

    if (lstat(fullpath, &statbuf) < 0) { /* stat error */
        return (func(fullpath, &statbuf, FTW_NS));
    }

    if (S_ISDIR(statbuf.st_mode) == 0) { /* not a directory */
        return (func(fullpath, &statbuf, FTW_F));
    }

    /*
     * It's a directory. First call func() for the directory,
     * then process each filename in the directory.
     */

    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0) { /* ndir++ */
        return (ret);
    }

    n = strlen(fullpath);
    if (n + static_cast<size_t>(NAME_MAX) + 2 > pathlen) { /* expand path buffer */
        pathlen *= 2;
        if ((fullpath = static_cast<char *>(realloc(fullpath, pathlen))) == nullptr) {
            err_sys("realloc failed");
        }
    }
    fullpath[n++] = '/';
    fullpath[n] = 0;

    if ((dp = opendir(fullpath)) == nullptr) { /* can't read diretory */
        return (func(fullpath, &statbuf, FTW_DNR));
    }

    while ((dirp = readdir(dp)) != nullptr) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        strcpy(&fullpath[n], dirp->d_name); /* append name after "/" */
        if ((ret = dopath(func)) != 0)
            break; /* recursive */
    }

    fullpath[n - 1] = 0;
    if (closedir(dp) < 0) {
        err_ret("can't close directory %s", fullpath);
    }

    return (ret);
}

static int myfunc(const char *pathname, struct stat *statptr, int type) {
    switch (type) {
    case FTW_F:
        switch (statptr->st_mode & S_IFMT) {
        case S_IFREG:
            nreg++;
            break;
        case S_IFBLK:
            nblk++;
            break;
        case S_IFCHR:
            nchr++;
            break;
        case S_IFIFO:
            nfifo++;
            break;
        case S_IFLNK:
            nslink++;
            break;
        case S_IFSOCK:
            nsock++;
            break;
        case S_IFDIR:
            err_dump("for S_IFDIR for %s", pathname);
        }
        break;
    case FTW_D:
        ndir++;
        break;
    case FTW_DNR:
        err_ret("can't read directory %s", pathname);
        break;
    case FTW_NS:
        err_ret("stat error for %s", pathname);
        break;
    default:
        err_dump("unknown type %d for pathname %s", type, pathname);
    }
    return 0;
}