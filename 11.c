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


#define BUFSIZE 1024


int main(void) {
    const char* filename = "data.txt";
    int fd = open(filename, O_RDWR | O_CREAT, 0644); // хотим и читать, и писать
    // блокируем эксклюзивной блокировкой 
    flock(fd, LOCK_EX);
    // обновляем значение в data.txt
    char buffer[BUFSIZE];
    read(fd, buffer, sizeof(buffer));
    int cnt = atoi(buffer);
    cnt++;
    char* newcnt;
    asprintf(&newcnt, "%d", cnt);
    // записываем новое значение, затирая старое
    pwrite(fd, newcnt, sizeof(newcnt), 0);
    // снимаем блокировку 
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}