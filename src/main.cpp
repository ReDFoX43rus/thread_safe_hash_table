#include "hash_table.h"

#include <pthread.h>
#include <iostream>

using namespace std;

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

void *handleRandoms(void* arg){
    thread_arg_t *info = (thread_arg_t*)arg;

    int action = rand()%2;

    for(int i = 0; i < info->to; i++){
        if(action)
            ht.Add(rand());
        else
            ht.Remove(rand());
    }

    return nullptr;
}

void handleAll(int from, int to, bool add){
    int n = to - from + 1;

    pthread_t *threads = new pthread_t[n];
    thread_arg_t *infos = new thread_arg_t[n];

    for(int i = 0; i < n; i++){
        infos[i].from = i + from;
        infos[i].to = i + from;
        infos[i].step = 1;

        pthread_create(&threads[i], NULL, add ? adder : remover, (void*)&infos[i]);
    }

    for(int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }


    bool all = true, any = false;
    for(int i = from; i <= to; i++){
        if(!ht.Find(i)){
            all = false;
        } else any = true;
    }

    cout << "Handle: all " << all << " any: " << any << endl;

    delete[] infos;
    delete[] threads;
}

int main(){
    ht.SetHashFunction(&HashFunction);

    handleAll(-50, 50, true);
    ht.Print();
    handleAll(-50, 50, false);
    ht.Print();

    int n = 20;

    pthread_t *threads = new pthread_t[n];
    thread_arg_t arg = {
        .from = 0,
        .to = 20,
        .step = 0
    };

    for(int i = 0; i < n; i++){
        pthread_create(&threads[i], NULL, handleRandoms, (void*)&arg);
    }

    for(int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }

    ht.Print();

    return 0;
}