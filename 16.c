#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
// Link with -lrt !!!

int main(void) {
    const char* queue_name = "/test-mqueue";
    // Create queue or open existing one:
    mqd_t queue_fd = mq_open(queue_name, O_RDWR | O_NONBLOCK | O_CREAT, 0600, NULL);
    if(queue_fd == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Send test message:
    if(mq_send(queue_fd, "hello", strlen("hello"), 0) < 0) {
        perror("failed to send another message");
    }

    // Get queue info and display it:
    struct mq_attr m_info;
    mq_getattr(queue_fd, &m_info);
    printf("Flags: %ld\n", m_info.mq_flags);
    printf("Max. # message on queue: %ld\n", m_info.mq_maxmsg);
    printf("Max. message size (bytes): %ld\n", m_info.mq_msgsize);
    printf("# of messages currently in queue: %ld\n", m_info.mq_curmsgs);
    
    // Cleanup:
    mq_close(queue_fd);
    mq_unlink(queue_name);
    return 0;
}