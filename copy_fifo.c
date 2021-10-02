#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


#define DEFAULT_CHUNK   262144


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
		
	*/
	// права источника
	struct stat sb;
	if(lstat(argv[1], &sb) == -1) {
		perror("lstat");
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
            if(mkfifo(argv[2], sb.st_mode) == -1) { // 777 = r+w+x for all users
                fprintf(stderr, "Failure to create FIFO file at %s\n", argv[2]);
                exit(EXIT_FAILURE);
            }
        } else {
            perror("lstat dst_file");
            exit(EXIT_FAILURE);
        }
    }
	return EXIT_SUCCESS;
}
