#include <pthread.h>
#include <stdio.h>

struct char_print_parms {
    char c;
    int cnt;
};

void* char_print(void* params) {
    struct char_print_parms* p = 
    (struct char_print_parms*)params;
    for(int i = 0; i < p->cnt; i++) {
        fputc(p->c, stderr);
    }
    return NULL;
}

int main(void) {
    pthread_t thread1_id, thread2_id;
    struct char_print_parms thread1_args, thread2_args;
    thread1_args.c = 'x';
    thread1_args.cnt = 30000;
    pthread_create(&thread1_id, NULL, &char_print, &thread1_args);
    // завершения потоков нужно дожидаться!
    pthread_join(thread1_id, NULL);
    thread2_args.c = '0';
    thread2_args.cnt = 20000;
    pthread_create(&thread2_id, NULL, &char_print, &thread2_args);
    pthread_join(thread2_id, NULL);
    return 0;
}