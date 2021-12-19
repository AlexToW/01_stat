/*
    14.2
*/ 
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>



int main(void) {
    int pipe_fds[2];
    if(pipe(pipe_fds) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t child_id = fork();
    if(child_id < 0) {
        perror("fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return 2;
    }

    if(child_id == 0) {
        close(pipe_fds[0]);

        if(dup2(pipe_fds[1], fileno(stdout)) < 0) {
            perror("dup2");
            close(pipe_fds[1]);
            return 3;
        }
        close(pipe_fds[1]);
        execlp("last", "last", NULL);
        // если вернуло управление, значит что-то не так
        perror("execlp");
        return 4;
    }

    close(pipe_fds[1]);
    execlp("wc", "wc", "-l", NULL);
    // если вернуло управление, значит что-то не так
    perror("execlp wc -l");
    return 0;
}