#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


#define DEFAULT_CHUNK   262144
#define O_PATH          010000000
#define PATH_MAX        4096


int copy_fifo(char* fifo_name);
int copy_blk_chr(char* blk_name, mode_t type);
int copy_symlink(char* src_link, char* dst_link);
int copy_reg(int src_fd, int dest_fd);
ssize_t write_all(int fd, const void *buf, size_t count);


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


int copy_reg(int src_fd, int dest_fd) {
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


int copy_fifo(char* fifo_dest_name) {
    struct stat sb;
    if(lstat(fifo_dest_name, &sb) == -1) {     // хотим обработать именно случай ошибки
        if(errno == ENOENT && strlen(fifo_dest_name) != 0) {
            // destination_file doesn't exist
            if(mkfifo(fifo_dest_name, sb.st_mode) == -1) {
                fprintf(stderr, "Failure to create FIFO %s\n", fifo_dest_name);
                return 3;
            }
        }
    }
    return 0;
}


int copy_blk_chr(char* blk_name, mode_t type) {
    struct stat sb;
    if(lstat(blk_name, &sb) == -1 && strlen(blk_name) != 0) {
        // то есть файл argv[2] не существует, будем создавать
        if(type == S_IFBLK) {
            if(mknod(blk_name, S_IFBLK | DEFFILEMODE, 0) == -1) { // DEFFILEMODE -- 0666
                return 3;
            }
        } else {
            if(mknod(blk_name, S_IFCHR | DEFFILEMODE, 0) == -1) {
                return 4;
            }
        }
    }
    return 0;
}

int copy_symlink(char* src_link, char* dst_link) {
    char* linkname = (char*)malloc(PATH_MAX);
    if(linkname == NULL) {
        perror("No memory");
        return 5;
    }
    ssize_t r = readlink(src_link, linkname, PATH_MAX);
    if(r == -1) {
        perror("readlink");
        free(linkname);
        return 6;
    }
    linkname[r] = '\0';
    if(symlink(linkname, dst_link) != 0) {
        perror("symlink");
        free(linkname);
        return 7;
    }
    free(linkname);
    return 0;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Bad argc: %d\n", argc);
        exit(EXIT_FAILURE);
    }

    struct stat sb_src;
    if(lstat(argv[1], &sb_src) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }
    mode_t type = sb_src.st_mode & S_IFMT;
    if(type == S_IFIFO) {
        if(copy_fifo(argv[2]) != 0) {
            perror("Copy FIFO failure");
            exit(EXIT_FAILURE);
        }
    } else if(type == S_IFLNK) {
        if(copy_symlink(argv[1], argv[2]) != 0) {
            perror("Copy symlink failure");
            exit(EXIT_FAILURE);
        }
    } else if(type == S_IFBLK || type == S_IFCHR) {
        if(copy_blk_chr(argv[2], type) != 0) {
            perror("Copy BLK/CHR failure");
            exit(EXIT_FAILURE);
        }
    } else if(type == S_IFREG) {
        int in_fd = open(argv[1], O_RDONLY);
        if (in_fd == -1) {
            perror("File open error");
            exit(EXIT_FAILURE);
        }

        int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, DEFFILEMODE);
        if (out_fd == -1) {
            perror("File open error");
            close(in_fd);
            exit(EXIT_FAILURE);
        }
        if(copy_reg(in_fd, out_fd) != 0) {
            fprintf(stderr, "Failed to copy to %s from %s", argv[1], argv[2]);
            exit(EXIT_FAILURE);
        }
        if (close(in_fd)) {
            perror("File close error");
            exit(EXIT_FAILURE);
        }
        if (close(out_fd)) {
            perror("File close error");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}