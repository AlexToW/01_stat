#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


#define DEFAULT_CHUNK   262144


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
    char* data = (char*)malloc(DEFAULT_CHUNK);
    if(!data) {
        close(src_fd);
        close(dest_fd);
        return ENOMEM;
    }
    while(1) {
        bytes = read(src_fd, data, sizeof(data));
        if(bytes == -1) {
            err = errno;
            close(src_fd);
            close(dest_fd);
            return err;
        }
        if(bytes == 0) {
            break;
        }
        bytes = write_all(dest_fd, data, bytes);
        if(bytes < 0) {
            err = EIO;
            if(bytes == -1) {
                err = errno;
            }
            free(data);
            close(src_fd);
            close(dest_fd);
            return err;
        }
    }

    free(data);

    err = 0;
    if (close(src_fd)) {
        err = EIO;
    }
    if (close(dest_fd)) {
        err = EIO;
    }
    return err;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s paht text\n", argv[0]);
        return EXIT_FAILURE;
    }

    int in_fd = open(argv[1], O_RDONLY);
    if (in_fd == -1) {
        return errno;
    }

    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        int err = errno;
        close(in_fd);
        return err;
    }
    if(copy_file(in_fd, out_fd) != 0) {
        fprintf(stderr, "Failed to copy to %s from %s", argv[1], argv[2]);
        return 5;
    }
    return 0;
}