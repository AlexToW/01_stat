/*
    14.1
*/

#define _GNU_SOURCE
 
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>


void proc_info(const char* procname) {
    printf(
            "%s: PID %d, PPID %d, PGID %d, SID %d\n",
            procname, getpid(), getppid(), getpgid(0), getsid(0)
    );
}


int main(void) {
    pid_t child_id = fork();
    if(child_id < 0) {
        perror("fork");
        return 1;
    }
    /* code for child process only */
    if(child_id == 0) {
        int file_fd = open("1.txt", O_WRONLY | /*O_TRUNC*/ O_APPEND | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); // assume that fd 1 is reused
        if(file_fd < 0) {
            perror("open");
            return 1;
        }
        if(dup2(file_fd, fileno(stdout)) < 0) {
            perror("dup2");
            close(file_fd);
            return 1;
        }
        close(file_fd);
    #if 0
        if(dup2(fileno(stderr), fileno(stdout)) < 0) {
            perror("dup2");
            return 1;
        }
    #endif
        proc_info("child");
        return 0;
    }
    proc_info("parent");
    wait(NULL);
    return 0;
}