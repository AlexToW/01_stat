#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <errno.h>


const char* device_type(struct stat* sb) {
    switch (sb->st_mode & S_IFMT) {
        case S_IFBLK:   return "block device";
        case S_IFCHR:   return "character device";
        case S_IFDIR:   return "directory";
        case S_IFIFO:   return "FIFO/channel";
        case S_IFLNK:   return "symbolic link";
        case S_IFREG:   return "regular file";
        case S_IFSOCK:  return "socket";
    }
    return "unknown";
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Incorrect argc: %d\n", argc);
        exit(EXIT_FAILURE);
    }

    struct stat sb;

    if(lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    printf("Тип файла:              %s\n", device_type(&sb));
    printf("инода:                  %ld\n", (long)sb.st_ino);
    printf("Режим доступа:          %lo (octal)\n", (unsigned long)sb.st_mode);
    printf("Кол-во ссылок:          %ld\n", (long)sb.st_nlink);
    printf("владелец:               UID=%ld\tGID=%ld\n", (long)sb.st_uid, (long)sb.st_gid);
    printf("Предпочт. размер блока: %ld байт\n", (long)sb.st_blksize);
    printf("Размер файла:           %lld байт\n", (long long)sb.st_size);
    printf("Выделено блоков:        %lld\n", (long long)sb.st_blocks);

    struct tm* current_time = (struct tm*)malloc(sizeof(struct tm));
    char* buf = (char*)malloc(26 * sizeof(char));

    current_time = gmtime_r(&sb.st_ctime, current_time);
    printf("C_TIME                  %s", asctime_r(current_time, buf));

    current_time = gmtime_r(&sb.st_atime, current_time);
    printf("A_TIME                  %s", asctime_r(current_time, buf));

    current_time = gmtime_r(&sb.st_mtime, current_time);
    printf("M_TIME                  %s", asctime_r(current_time, buf));

    free(current_time);
    free(buf);
    return 0;
}