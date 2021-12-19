#include <stdio.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/statvfs.h>
// #include "/usr/include/linux/magic.h"

#if 0
struct statfs {
               __fsword_t f_type;    Type of filesystem (see below) 
               __fsword_t f_bsize;    Optimal transfer block size 
               fsblkcnt_t f_blocks;   Total data blocks in filesystem 
               fsblkcnt_t f_bfree;    Free blocks in filesystem 
               fsblkcnt_t f_bavail;  Free blocks available to
                                        unprivileged user 
               fsfilcnt_t f_files;    Total file nodes in filesystem 
               fsfilcnt_t f_ffree;    Free file nodes in filesystem 
               fsid_t     f_fsid;     Filesystem ID 
               __fsword_t f_namelen;  Maximum length of filenames 
               __fsword_t f_frsize;   Fragment size (since Linux 2.6) 
               __fsword_t f_flags;    Mount flags of filesystem
                                        (since Linux 2.6.36) 
               __fsword_t f_spare[xxx];
                                Padding bytes reserved for future use 
           };
#endif


void print_info(struct statvfs* sb) {
    printf("Total:      %ju\n", (uintmax_t)sb->f_blocks * sb->f_bsize);
    printf("Total:      %ju\n", (uintmax_t)sb->f_blocks * sb->f_frsize);
    printf("Available:  %ju\n", (uintmax_t)sb->f_bavail * sb->f_bsize);
    printf("Available:  %ju\n", (uintmax_t)sb->f_bavail * sb->f_frsize);
    printf("Used:       %ju\n", (uintmax_t)sb->f_bsize * (sb->f_blocks  - sb->f_bavail)); // total - available
    printf("Used:       %ju\n", (uintmax_t)sb->f_frsize * (sb->f_blocks  - sb->f_bfree));
}


int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Bad argc(%d)\n", argc);
        exit(EXIT_FAILURE);
    }
    struct statvfs sb;
    for(int i = 1; i < argc; i++) {
        if(statvfs(argv[i], &sb) == 0) {
            print_info(&sb);
        }
    }
    return 0;
}