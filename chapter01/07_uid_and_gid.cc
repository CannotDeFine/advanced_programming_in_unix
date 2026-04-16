#include "apue.h"
#include <unistd.h>

// 口令文件/etc/passwd或者/etc/shadow:登录名和用户ID之间的映射关系
// 组文件/etc/group:组名和组ID之间的映射关系
// 一般用户创建时会创建自己用户名命名的组

int main() {
    printf("uid = %d, gid = %d\n", getuid(), getgid());
    exit(0);
}