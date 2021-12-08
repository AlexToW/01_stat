/*
    Написать программу, которая в дочернем процессе
    запускает gzip, работая с ним через два pipe'а
    один процесс -- gzip (дочерний)
    родительский -- наша программа -- кормит gzip случайными 
    данными, читает его выход (инчае pipe забъётся и 
    gzip заснет)
    (сжатые), и выдаёт скорость (clock_gettime)
    poll/epoll/select  -- чтобы читать и писать в олном процессе
    
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
        return 1;
    }

    if(child_id == 0) {
        close(pipe_fds[0]);
        if(dup2(pipe_fds[1], fileno(stdout)) < 0) {
            perror("dup2");
            close(pipe_fds[1]);
            return 1;
        }
        close(pipe_fds[1]);
        execlp("gzip", "gzip", "<", "1.txt", NULL);
        perror("failed to exec 'gzip'");
        return 1;
    }
    close(pipe_fds[1]);
    if(dup2(pipe_fds[0], fileno(stdin)) < 0) {
        perror("dup2");
        close(pipe_fds[0]);
        return 1;
    }
    close(pipe_fds[0]);
    execlp("wc", "wc", "-l", NULL);
    perror("failed to exec 'wc -l'");
    return 0;
}