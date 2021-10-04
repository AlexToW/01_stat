#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//int mknod(const char *pathname, mode_t mode, dev_t dev);
//-- копирование блочное/символьное устройство (S_IFBLK, S_IFCHR) (mknod) 

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Bad argc: %d\n", argc);
        return 1;
    }
    struct stat sb;
    if(lstat(argv[1], &sb) == -1) {
        return 2;
    }
    mode_t type = sb.st_mode & S_IFMT;
    if(type == S_IFBLK || type == S_IFCHR) {
        if(lstat(argv[2], &sb) == -1 && strlen(argv[2]) != 0) {
            // то есть файл argv[2] не существует, будем создавать
            if(type == S_IFBLK) {
                if(mknod(argv[2], S_IFBLK | 0777, 0) == -1) {
                    return 3;
                }
            } else {
                if(mknod(argv[2], S_IFCHR | 0777, 0) == -1) {
                    return 4;
                }
            }
        }
    }
    return 0;
}
