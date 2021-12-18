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
    printf("kill -s SIGKILL %d\n", getpid());
    struct sigaction sigact_struct = {
        .sa_handler = sig_handler,
        .sa_flags = SA_RESTART
    };
    sigemptyset(&sigact_struct.sa_mask);
    #if 0
    sigaction(SIGINT, &sigact_struct, NULL);
    sigaction(SIGQUIT, &sigact_struct, NULL);
    sigaction(SIGSTOP, &sigact_struct, NULL); // не работает, так как SIGSTOP поймать нельзя.
    sigaction(SIGTERM, &sigact_struct, NULL);
    sigaction(SIGTSTP, &sigact_struct, NULL);// ctrl + Z
    #endif
    for(int signal = 1; signal <= NSIG; signal++) {
        if(sigaction(signal, &sigact_struct, NULL) == -1) {
            perror("sigaction");
            psignal(signal, NULL);
        }
    }
    siginfo_t  info;
    sigset_t   mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGTSTP);
    while(1) {
        int signum = sigwaitinfo(&mask, &info);
        // printf("%d\n", signum);
        sig_handler(signum);
        printf("Процесс, пославший сигнал: %d, last_signal = %d (%s)\n", info.si_pid, g_last_signal, strsignal(g_last_signal));
    }
    return 0;
}