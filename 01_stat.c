#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <errno.h>


const char* device_type(const mode_t mode) {
    switch (mode & S_IFMT) {
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

    printf("File type:              %s\n", device_type(sb.st_mode));
    printf("INode:                  %ld\n", (long)sb.st_ino);
    printf("Access mode:            %lo (octal)\n", (unsigned long)sb.st_mode);
    printf("number of links:        %ld\n", (long)sb.st_nlink);
    printf("Owner:                  UID=%ld\tGID=%ld\n", (long)sb.st_uid, (long)sb.st_gid);
    printf("preferred block size:   %ld byte\n", (long)sb.st_blksize);
    printf("File size:              %lld byte\n", (long long)sb.st_size);
    printf("Allocated blocks:       %lld\n", (long long)sb.st_blocks);


    struct tm current_time;
    char buf[sizeof("Day Mon dd hh:mm:ss yyyy\n")];

    const char fmt[] = "%a %b %d %H:%M:%S %Y";
    current_time = *gmtime_r(&sb.st_ctime, &current_time);
    if(!strftime(buf, 26, fmt, &current_time)) {
        exit(EXIT_FAILURE);
    }
    printf("C_TIME                  %s\n", buf);
    current_time = *gmtime_r(&sb.st_atime, &current_time);
    if(!strftime(buf, 26, fmt, &current_time)){
        exit(EXIT_FAILURE);
    }
    printf("A_TIME                  %s\n", buf);
    current_time = *gmtime_r(&sb.st_mtime, &current_time);
    if(!strftime(buf, 26, fmt, &current_time)) {
        exit(EXIT_FAILURE);
    }
    printf("M_TIME                  %s\n", buf);

    return 0;
}