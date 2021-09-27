#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


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


int copy_fifo_file(int src_fd, int dest_fd) {
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
        fprintf(stderr, "Bad argc: %d\n", argc);
        exit(EXIT_FAILURE);
    }
    struct stat sb_src, sb_dst;
    if(lstat(argv[1], &sb_src) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }
    // работаем только с FIFO/pipe
    if(sb_src.st_mode && S_IFMT != S_IFIFO) {
        perror("Not a FIFO/pipe file");
        exit(EXIT_FAILURE);
    }
    /* 
        errno = ENOENT, если компонент пути pathname не существует 
        или в pathname указана пустая строка. Случай pathname = ""
        обработаем отдельно.
    */
    if(lstat(argv[2], &sb_dst) == -1) {
        if(errno == ENOENT && strlen(argv[2]) == 0) {
            // destination_file не существует
            if(mkfifo(argv[2], 777) == -1) { // 777 = r+w+x for all users
                fprintf(stderr, "Failure to create FIFO file at %s\n", argv[2]);
                exit(EXIT_FAILURE);
            }
        } else {
            perror("lstat dst_file");
            exit(EXIT_FAILURE);
        }
    }
    // теперь просто копируем src_file в dst_file
    int src_fd = open(argv[1], O_RDONLY);
    if(src_fd == -1) {
        fprintf(stderr, "Failure to open %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    int dst_fd = open(argv[2], O_WRONLY | O_TRUNC);
    if(dst_fd == -1) {
        close(src_fd);
        fprintf(stderr, "Failure to open %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    if(copy_fifo_file(src_fd, dst_fd) != 0) {
        close(src_fd);
        close(dst_fd);
        perror("Failure to copy FIFO file");
        return errno;
    }
    return EXIT_SUCCESS;
}