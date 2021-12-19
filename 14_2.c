/*
    14.1
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

    
    execlp(
        // path to executable
        //"grep", 
        "grep"
        // argv
        "grep", argc > 1 ? argv[1] : "model name", "/proc/cpuinfo", NULL
        );

#if 0
    printf("тест 2");
    execlp("echo", "echo", "1", "abc", "тест", NULL);
#endif
    perror("failed to exec grep"); // так как в штатном случае execlp управление не возвращает
    return 0;
}