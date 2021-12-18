#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


#define DEFAULT_CHUNK   (256 * 1024)


ssize_t write_all(int fd, const void *buf, size_t count) { // signed size_t
    /* будет вызывать write до тех пор, пока не возникнет ошибка, либо пока не запишем всё) */
    size_t bytes_written = 0;
    while(bytes_written < count) {
        ssize_t res = write(fd, buf + bytes_written, count - bytes_written);
        if(res < 0) {
            return res;
        }
        bytes_written += res;
    }
    return (ssize_t)bytes_written;
}


int copy_file(int src_fd, int dest_fd) {
    ssize_t bytes = 0;
    int err;
    char* data = (char*)malloc(DEFAULT_CHUNK); // вынести 
    if(!data) {
        return ENOMEM;
    }
    while(1) {
        bytes = read(src_fd, data, DEFAULT_CHUNK);
        if(bytes == -1) {
            err = errno;
            perror("read");
            return err;
        }
        if(bytes == 0) {
            break;
        }
        bytes = write_all(dest_fd, data, bytes);
        if(bytes < 0) {
            err = errno;
            perror("read");
            free(data);
            return err;
        }
    }
    free(data);
    return 0;
}


int main(int argc, char* argv[]) {
    int result = 0;
    if(argc != 3) {
        fprintf(stderr, "Usage: %s path text\n", argv[0]);
        return 1;
    }

    int in_fd = open(argv[1], O_RDONLY);
    if (in_fd == -1) {
        perror("File open error");
        return 2;
    }

    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        perror("File open error");
        close(in_fd);
        return 3;
    }
    if(copy_file(in_fd, out_fd) != 0) {
        fprintf(stderr, "Failed to copy to %s from %s", argv[1], argv[2]);
        result = 4;
    }
    if (close(in_fd)) {
        perror("File close error");
        result = 5;
    }
    if (close(out_fd)) {
        perror("File close error");
        result = 6;
    }
    return result;
}