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



int main(int argc, char* argv[]) {
    // grep 'model name' /proc/cpuinfo -- хотим сделать так
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