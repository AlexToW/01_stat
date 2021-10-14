/*
    aka 04_metadata + copy UID/GID
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>


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
    return 0;
}


int copy_metadata(char* src_file, char* dest_file, int dest_fd) {
    struct stat sb_src;
    if(stat(src_file, &sb_src) == 0) {
        /* copy mode */
        if(fchmod(dest_fd, sb_src.st_mode) != 0) {
            perror("fchmod");
            return 1;
        }
        /* copy atime, mtime */
        const struct timespec times[2] = {sb_src.st_atim, sb_src.st_mtim};
        if(futimens(dest_fd, times) != 0) {
            perror("futimens");
            return 2;
        }
        /* copy UID, GID*/
        if(lchown(dest_file, sb_src.st_uid, sb_src.st_gid) != 0) {
            perror("fchown");
            return 3;
        }
    }
    return 0;
}


int main(int argc, char* argv[]) {
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
        return 4;
    }
    if (close(in_fd)) {
        perror("File close error");
        return 6;
    }
    if(copy_metadata(argv[1], argv[2],  out_fd) != 0) {
        perror("Metadata copy failure");
        return 5;
    }
    if (close(out_fd)) {
        perror("File close error");
        return 7;
    }
    return 0;
}