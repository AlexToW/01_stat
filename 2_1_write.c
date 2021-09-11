#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


ssize_t write_all(int fd, const void *buf, size_t count) { // signed size_t
    // будет вызывать write до тех пор, пока не возникнет ошибка, либо пока не запишем всё)
    size_t bytes_written = 0;
    while(bytes_written < count) {
        ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);
        if(res < 0) {
            return res;
        }
        bytes_written += res;
    }
    return (ssize_t)bytes_written;
}


int main(int argc, char* argv[]) {
    
    if(argc != 3) {
        fprintf(stderr, "Usage: %s paht text\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // user -- group -- other
    if(fd < 0) {
        perror("Failed to open file to writing");
        return 2;
    }


    if(write_all(fd, argv[2], strlen(argv[2])) < 0) {
        perror("Failed to write");
        close(fd);
        return 3;
    }


    if(close(fd) < 0) {
        perror("Failure during close");
        return 4;
    }
    
    return 0;
}