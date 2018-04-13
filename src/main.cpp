#include "hash_table.h"

#include <pthread.h>

#define THREADS_N 8

using IntTable = CHashTable<int>;
IntTable ht;

static int HashFunction(int value){
    return (value*2654435761) & 0xFF;
}

struct thread_arg_t{
    int from;
    int to;
    int step;
};

void *adder(void* arg){
    thread_arg_t *info = (thread_arg_t*)arg;
    int from = info->from;
    int to = info->to;
    int step = info->step;

    for(int i = from; i <= to; i += step)
        ht.Add(i);

    return nullptr;
}

void *remover(void* arg){
    thread_arg_t *info = (thread_arg_t*)arg;
    int from = info->from;
    int to = info->to;
    int step = info->step;

    for(int i = from; i <= to; i += step)
        ht.Remove(i);

    return nullptr;
}

int main(){
    ht.SetHashFunction(&HashFunction);

    for(int i = 0; i < 10000; i += 3)
        ht.Add(i);

    ht.Print();

    for(int i = 0; i < 10000; i += 6)
        ht.Remove(i);

    // ht.Print();

    pthread_t threads[THREADS_N];
    thread_arg_t args[THREADS_N];

    for(int i = 0; i < THREADS_N; i++){
        args[i].from = -10000 + i * 5000;
        args[i].to = args[i].from + 5500 * (i + 2);
        args[i].step = 4;

        pthread_create(&threads[i], NULL, adder, &args[i]);
    }

    for(int i = 0; i < THREADS_N; i++){
        pthread_join(threads[i], NULL);
    }

    ht.Print();

    return 0;
}