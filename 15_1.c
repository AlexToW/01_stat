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
        // .sa_flags = SA_RESTART
        .sa_flags = SA_SIGINFO
    };
    sigemptyset(&sigact_struct.sa_mask);
    // struct siginfo_t* info = calloc(1, sizeof(struct siginfo_t));
    #if 0
    sigaction(SIGINT, &sigact_struct, NULL);
    sigaction(SIGQUIT, &sigact_struct, NULL);
    sigaction(SIGSTOP, &sigact_struct, NULL); // не работает, так как SIGSTOP поймать нельзя.
    sigaction(SIGTERM, &sigact_struct, NULL);
    sigaction(SIGTSTP, &sigact_struct, NULL);// ctrl + Z
    #endif
    for(int signal = 1; signal <= NSIG; signal++) {
        if(sigaction(signal, &sigact_struct, NULL)) {
            perror("sigaction");
            psignal(signal, NULL);
        }
    }
    while(1) {
        pause();
        printf(" last_signal = %d (%s)\n", g_last_signal, strsignal(g_last_signal));
    }
    return 0;
}