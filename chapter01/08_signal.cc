#include "apue.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

static void sig_int(int); 

int main(void) {
    char buffer[MAXLINE];
    pid_t pid;
    int status;

    if (signal(SIGINT, sig_int) == SIG_ERR) err_sys("signal error.");

    printf("%% ");
    while((fgets(buffer, MAXLINE, stdin)) != NULL) {
        if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = 0;

        if ((pid = fork()) < 0) {
            err_sys("fork error.");
        } else if (pid == 0) { /* child */
            execlp(buffer, buffer, (char*)0);
            err_ret("could not exec %s.", buffer);
            exit(127);
        }

        /* parent */
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            err_sys("waitpid error.");
        }

        printf("%% ");
    }

    exit(0);
}

void sig_int(int signo) {
    (void)signo;
    const char msg[] = "\ninterrupt\n% ";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}
