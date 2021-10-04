#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Bad argc: %d\n", argc);
        return 1;
    }
    struct stat sb_src, sb_dst;
    if(lstat(argv[1], &sb_src) == -1) {
        perror("lstat");
        return 2;
    }
    // работаем только с FIFO/pipe
    if((sb_src.st_mode & S_IFMT) == S_IFIFO) {
        /* 
            errno = ENOENT, если компонент пути pathname не существует 
            или в pathname указана пустая строка. Случай pathname = ""
            обработаем отдельно.
        */
        if(lstat(argv[2], &sb_dst) == -1) {     // хотим обработать именно случай ошибки
            if(errno == ENOENT && strlen(argv[2]) != 0) {
                // destination_file doesn't exist
                if(mkfifo(argv[2], sb_src.st_mode) == -1) {
                    fprintf(stderr, "Failure to create FIFO %s\n", argv[2]);
                    return 3;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
