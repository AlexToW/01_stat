#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
// Link with -lrt !!!

int main(int argc, char* argv[]) { // имя очереди и что в нее написать
    if(argc != 3) {
        perror("argc");
        return 1;
    }
    int result = 0;
    // Create queue or open existing one:
    mqd_t queue_fd = mq_open(argv[1], O_WRONLY);
    if(queue_fd == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Send test message:
    if(mq_send(queue_fd, argv[2], strlen(argv[2]), 0) < 0) {
        perror("failed to send your message");
        result = 1;
    }

    mq_close(queue_fd);
    return result;
}