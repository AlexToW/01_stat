
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 4096

int reg_file_cpy(char *source_path, char  *target_dir_path, char *file_name) {
    int src_fd = open(source_path, O_RDONLY);
    if(src_fd == -1) {
        perror("Failed to open source file");
        exit(EXIT_FAILURE);
    }
    struct stat stat_buf;
    if(lstat(source_path, &stat_buf) == -1) {
        perror("Failed to stat");
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
        printf("%s\n%s\n", target_dir_path, file_name);
    #endif
    char *target_path;
    if(asprintf(&target_path, "%s/%s", target_dir_path, file_name) < 0) {
        printf("Failed to asprintf");
        exit(EXIT_FAILURE);
    }
    int dst_fd = open(target_path, O_WRONLY|O_CREAT|O_TRUNC, stat_buf.st_mode);
    if (dst_fd == -1) {
        perror("Failed to open target file");
        exit(EXIT_FAILURE);
    }
    if (fchmod(dst_fd, stat_buf.st_mode) == -1) {
        perror("Failed to fchmod");
        exit(EXIT_FAILURE);
    }
    struct timespec am_time[2] = {stat_buf.st_atim,stat_buf.st_mtim};
    while(1){
        uint8_t buf[BUFSIZE];
        ssize_t buf_size = read(src_fd, buf, sizeof(buf));

        if(buf_size == -1) {
            perror("Failed to read a block");
            close(src_fd);
            close(dst_fd);
        }
        if (buf_size == 0){
            break;
        }
        size_t local_buf_size = (size_t)buf_size;
        size_t bytes_written = 0;
        while (bytes_written < local_buf_size){
            ssize_t write_result = write(dst_fd, &buf[bytes_written], local_buf_size - bytes_written);
            fsync(dst_fd);
            if (write_result == -1){
                perror("Failed to write");
                close(src_fd);
                close(dst_fd);
                return -1;
            }
            bytes_written += (size_t)write_result;
        }

    }
    int err = 0;
    if (fsync(dst_fd)) {
        perror("Failed to fsync");
        err = -1;
    }
    if (futimens(dst_fd, am_time) == -1) {
        perror("Failed to futimens");
        err = -1;
    }
    close(src_fd);
    close(dst_fd);
    return err;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: bad argc (%d)\n", argc);
        exit(EXIT_FAILURE);
    }
    char* dirname = strrchr(argv[1], '/');
    if(!dirname) {
        fprintf(stderr, "Usage strrchr; bad argv[1]: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    DIR* dir_str = opendir(argv[1]);
    if(!dir_str) {
        perror("Failure with opendir");
        exit(EXIT_FAILURE);
    }

    int dir_fd;
    if((dir_fd = dirfd(dir_str)) == -1) {
        perror("Failure dirfd");
        exit(EXIT_FAILURE);
    }

    char* target;
    if(asprintf(&target, "%s%s", argv[2], dirname) < 0) {
        perror("asprintf");
        exit(EXIT_FAILURE);
    }

    if(mkdir(target, S_IRUSR|S_IWUSR|S_IEXEC|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) == -1){
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    struct dirent *dir;
    while ((dir = readdir(dir_str)) != NULL) {
        char *source_path;
        struct stat stat_buf;
        asprintf(&source_path, "%s/%s", argv[1], dir->d_name);
        if (lstat(source_path, &stat_buf) == -1) {
            perror("Failed to lstat:");
            continue;
        }
        if (S_ISREG(stat_buf.st_mode)) {
            if (reg_file_cpy(source_path, target, dir->d_name) == -1) {
                perror("Failed to copy ");
            }
            else
                printf("%s has been copied\n", dir->d_name);
        }
        else if (S_ISDIR(stat_buf.st_mode)) {
            char *target_dir_path;
            asprintf(&target_dir_path, "%s/%s", target, dir->d_name);
            if (mkdir(target_dir_path, stat_buf.st_mode) == -1) {
                printf("%s has been copied\n", dir->d_name);
            }
        }
        free(source_path);
    }
    free(target);

    close(dir_fd);
    closedir(dir_str);
    return 1;
}