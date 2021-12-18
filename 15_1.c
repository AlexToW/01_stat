/*
    убивать "kill -s SIGKILL <pid>"
*/

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


volatile int g_last_signal; // перечитывать заново

void sig_handler(int signum) {
    g_last_signal = signum;
}


int main(void) {
    printf("PID: %d\n", getpid());
    struct sigaction sigact_struct = {
        .sa_handler = sig_handler,
        .sa_flags = SA_RESTART
    };
    sigaction(SIGINT, &sigact_struct, NULL);
    sigaction(SIGQUIT, &sigact_struct, NULL);
    sigaction(SIGSTOP, &sigact_struct, NULL); // не работает, так как SIGSTOP поймать нельзя.
    sigaction(SIGTERM, &sigact_struct, NULL);
    sigaction(SIGTSTP, &sigact_struct, NULL);// ctrl + Z
    
    while(1) {
        pause();
        printf(" last_signal = %d (%s)\n", g_last_signal, strsignal(g_last_signal));
    }
    return 0;
}