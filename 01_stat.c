#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <errno.h>


const char* device_type(struct stat* sb) {
    switch (sb->st_mode & S_IFMT) {
        case S_IFBLK:   return "блочное устройство";
        case S_IFCHR:   return "символьное устройство";
        case S_IFDIR:   return "каталог";
        case S_IFIFO:   return "FIFO/канал";
        case S_IFLNK:   return "символьная ссылка";
        case S_IFREG:   return "обычный файл";
        case S_IFSOCK:  return "сокет";
    }
    return "неизвестно";
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

    return 0;
}