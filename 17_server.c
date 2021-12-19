#include <stdio.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <mqueue.h> 
#include <signal.h>  
#include <errno.h>  
#include <stdlib.h> 
#include <string.h>

volatile int last_signal;

void handler(int signum) {
    last_signal++;
    last_signal = signum;
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: bad argc(%d), exepcted: 2", argc);
        return 1;
    }
    struct sigaction sig_struct;
    sigemptyset(&sig_struct.sa_mask);
    sig_struct.sa_handler = handler;
    sig_struct.sa_flags = 0;
    
    if(sigaction(SIGINT, &sig_struct, NULL) < 0) {
        perror("sigaction");
        return 2;
    }

    const char* queue_name = argv[1];
    mqd_t queue_fd = mq_open(queue_name, O_CREAT | O_EXCL | O_RDONLY, 0622, NULL);
    if(queue_fd == (mqd_t) - 1) {
        perror("mq_open");
        return 3;
    }

    struct mq_attr attr;
    if(mq_getattr(queue_fd, &attr) < 0) {
        perror("mq_getattr");
        return 4;
    }

    long msg_size = attr.mq_msgsize;
    char* buff = malloc(msg_size + 1);
    if(buff == NULL) {
        perror("malloc");
        return 5;
    }

    ssize_t read = 0;
    unsigned prio = 0;
    while(1) {
        read = mq_receive(queue_fd, buff, msg_size, &prio);
        if(read < 0) {
            break;
        }
        buff[read] = '\0';
        printf("%s\n", buff);
    }

    if(errno != EINTR && errno != EAGAIN) {
        perror("mq_recieve");
    }
    free(buff);

    if(mq_close(queue_fd) < 0) {
        perror("mq_close");
        return 6;
    }
    if(mq_unlink(queue_name) < 0) {
        perror("mq_unlink");
        return 7;
    }
    return 0;
}