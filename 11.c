/*
    подсчитывает кол-во запусков себя
*/
#define _GNU_SOURCE // чтобы asprintf работал
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>


#define BUFSIZE 1024
#define DEBUG


int main(void) {
    const char* filename = "data.txt";
    int fd = open(filename, O_RDWR | O_CREAT, 0644); // хотим и читать, и писать
    if(fd == -1) {
        perror("Failed to open file");
        return 1;
    }
    // блокируем эксклюзивной блокировкой 
    if(flock(fd, LOCK_EX) == -1) {
        perror("Failed to flock");
        return 2;
    }
    // обновляем значение в data.txt
    char buffer[BUFSIZE];
    if(read(fd, buffer, sizeof(buffer)) == -1) {
        perror("Failed to read");
        close(fd);
        return 3;
    }
    int cnt = atoi(buffer);
    cnt++;
    #ifdef DEBUG
        printf("currrent cnt-1: %d\n", cnt);
    #endif
    char* newcnt;
    if(asprintf(&newcnt, "%d", cnt) < 0) {
        perror("Failed to asprnitf");
        close(fd);
        free(newcnt);
        return 4;
    }
    // записываем новое значение, затирая старое
    if(pwrite(fd, newcnt, strlen(newcnt), 0) == -1) {
        perror("Failed to pwrite");
        close(fd);
        free(newcnt);
        return 5;
    }
    free(newcnt);
    // снимаем блокировку 
    if(flock(fd, LOCK_UN) == -1) {
        perror("Failed to flock: unlock");
        close(fd);
        return 6;
    }
    close(fd);
    return 0;
}