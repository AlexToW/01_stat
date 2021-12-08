#define _GNU_SOURCE
/*
    еще 13
*/
 
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


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
        proc_info("child");
        sleep(1);
        // this code is executed in child process onlly
        proc_info("child");
        kill(getpid(), SIGTERM);
        return 0;
    }
    // this code is executed in parent processes
    proc_info("parent");
    int status;
    pid_t res = wait(&status);
    if(WIFEXITED(status)) {  // завершился добровольно
        printf("child with PID %d has exited normally with status %d\n", res, WEXITSTATUS(status));
    } else if(WIFSIGNALED(status)){ // процесс прибило сигналом
        printf("child with PID = %d has killed by signal %d (%s)\n", res, WTERMSIG(status), strsignal(WTERMSIG(status)));
        // здесь ответвление для 13.1
        #ifdef WCOREDUMP
            if(WCOREDUMP(status)) {
                printf("core dump: pid = %d\n", getpid());
            }
        #endif
    } else {
        printf("child with PID = %d has exited with termination status %d\n", res, status);
    }
    // this code is executed in both processes
    return 0;
}