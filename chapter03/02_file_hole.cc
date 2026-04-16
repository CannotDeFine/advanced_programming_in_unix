#include "apue.h"
#include <fcntl.h>

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";

// 文件的偏移量大于当前文件的长度形成了文件空洞；
// 当定位超过文件尾端，对新写入的数据分配磁盘块；
// 原文件尾端到新开始写位置之间的部分不需要分配磁盘块。

int main(void) {
    int fd;

    if ((fd = creat("file.hole", FILE_MODE)) == -1) {
        err_sys("creat error.");
    }

    if ((write(fd, buf1, 10)) != 10) {
        err_sys("wtire error.");
    }
    
    /* offset = 10 */

    if ((lseek(fd, 16384, SEEK_SET)) == -1) {
        err_sys("lseek error.");
    }

    /* offset = 16384 */

    if ((write(fd, buf2, 10)) != 10) {
        err_sys("write error.");
    }

    /* offset = 16394 */

    exit(0);
}