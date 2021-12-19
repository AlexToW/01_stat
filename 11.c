/*
    подсчитывает кол-во запусков себя
*/
#define _GNU_SOURCE // для asprintf
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>


#define DEBUG


int main(void) {
    const char* filename = "data.txt";
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); // хотим и читать, и писать
    if(fd == -1) {
        perror("Failed to open file");
        return 1;
    }
    FILE* file = fdopen(fd, "r+");
    if(!file) {
        perror("fdopen");
        close(fd);
        exit(EXIT_FAILURE);
    }
    // блокируем эксклюзивной блокировкой 
    if(flock(fd, LOCK_EX) == -1) {
        perror("Failed to flock");
        fclose(file);
        return 2;
    }
    // обновляем значение в data.txt
    int cnt = 0;
    if(!fscanf(file, "%d", &cnt)) {
        perror("Failed to fscanf");
        fclose(file);
        return 3;
    }
    cnt++;
    #ifdef DEBUG
        printf("currrent cnt-1: %d\n", cnt);
    #endif
    char* newcnt;
    
    if(asprintf(&newcnt, "%d", cnt) < 0) {
        perror("Failed to asprnitf");
        free(newcnt);
        fclose(file);
        return 4;
    }

    // записываем новое значение, затирая старое
    if(pwrite(fd, newcnt, strlen(newcnt), 0) == -1) {
        perror("Failed to pwrite");
        fclose(file);
        // free(newcnt);
        return 5;
    }
    free(newcnt);
    // снимаем блокировку 
    if(flock(fd, LOCK_UN) == -1) {
        perror("Failed to flock: unlock");
        fclose(file);
        return 6;
    }
    fclose(file);
    return 0;
}