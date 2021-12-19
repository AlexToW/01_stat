#include <stdio.h>
#include <pthread.h>
#include <errno.h>


#define N_THREADS 12
#define N 1000000

volatile int cnt = 0;

void* thread_foo(void* arg) {
    (void)arg;
    for(int i = 0; i < N; i++) {
        cnt++;
    }
    return NULL;
}


int main(void) {
    pthread_t threads[N_THREADS];
    for(int i = 0; i < N_THREADS; i++) {
        pthread_create(threads + i, NULL, &thread_foo, NULL);
        pthread_join(threads[i], NULL);
    }
    #if 0
    for(int i = 0; i < N_THREADS; i++) {
        errno = pthread_join(threads[i], NULL);
        printf("Current cnt = %d\n", cnt);
    }
    #endif
    printf("Result is %d\n", cnt);
    return 0;
}