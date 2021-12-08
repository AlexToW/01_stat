#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

// grep 'model name' /proc/cpuinfo | wc -l -- хотим такой конвеер
// если конвеер длиннее, то форки делать в main
int main(void) {
    // create pipe
    int pipe_fds[2]; // 0 -- aka stdout, 1 -- aka stdin
    if(pipe(pipe_fds) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t child_id = fork();
    if(child_id < 0) {
        perror("fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return 1;
    }

    // child process only
    if(child_id == 0) {
        // close unused reading endpoint of the pipe
        close(pipe_fds[0]);
        // redirect stdout to pipe
        if(dup2(pipe_fds[1], fileno(stdout)) < 0) {
            perror("dup2");
            close(pipe_fds[1]);
            return 1;
        }
        close(pipe_fds[1]);
        // execute 'grep'
        execlp("grep", "grep", "model name", "/proc/cpuinfo", NULL);
        // сам дочерний процесс нормально завершиться не может
        perror("failed to exec 'last'");
        return 1;
    }

    close(pipe_fds[1]);
    // redirect stdin from pipe
    if(dup2(pipe_fds[0], fileno(stdin)) < 0) {
        perror("dup2");
        close(pipe_fds[0]);
        return 1;
    }
    close(pipe_fds[0]);
    // execute 'wc -l'
    execlp("wc", "wc", "-l", NULL);
    perror("failed to exec 'wc -l'");
    return 1;
} 