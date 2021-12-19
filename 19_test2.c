#include <stdio.h>
#include <pthread.h>

void* prime_f(void* arg) {
    int candidate = 2;
    int n = *((int*)arg);
    while(1) {
        int is_prime = 1;
        for(int factor = 2; factor < candidate; factor++){
            if(candidate % factor == 0) {
                is_prime = 0;
                break;
            }
        }
        if(is_prime) {
            if(--n == 0) {
                return (void*)candidate;
            }
        }
        ++candidate;
    }
    return NULL;
}


int main(void) {
    pthread_t thread;
    int N = 5000;
    int prime;

    pthread_create(&thread, NULL, &prime_f, &N);
    // делаем, что хотим
    for(int i = 0; i < 5; i++) {
        printf("Hello, world!\n");
    }

    pthread_join(thread, (void*)&prime);
    printf("prime: %d\n", prime);
    // поток не должен ждать сам себя!
    if (!pthread_equal(pthread_self(), other_thread)) 
        pthread_join(other_thread, NULL);
}