#include "hash_table.h"

#include <thread>
#include <vector>
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

    thread_arg_t() {}

    thread_arg_t(int from, int to, int step){
        this->from = from;
        this->to = to;
        this->step = step;
    }
};

void adder(thread_arg_t info){
    int from = info.from;
    int to = info.to;
    int step = info.step;

    for(int i = from; i <= to; i += step)
        ht.Add(i);
}

void remover(thread_arg_t info){
    int from = info.from;
    int to = info.to;
    int step = info.step;

    for(int i = from; i <= to; i += step)
        ht.Remove(i);
}

void handleRandoms(thread_arg_t info){
    int action = rand()%2;

    for(int i = 0; i < info.to; i++){
        if(action)
            ht.Add(rand());
        else
            ht.Remove(rand());
    }
}

void handleAll(int from, int to, bool add){
    int n = to - from + 1;

    std::vector<std::thread> threads;
    for(int i = 0; i < n; i++){
        threads.push_back(std::thread(add ? adder : remover, thread_arg_t(i + from, i + from, 1)));
    }

    for(int i = 0; i < n; i++){
        threads[i].join();
    }    

    bool all = true, any = false;
    for(int i = from; i <= to; i++){
        if(!ht.Find(i)){
            all = false;
        } else any = true;
    }

    cout << "Handle: all " << all << " any: " << any << endl;
}

int main(){
    ht.SetHashFunction(&HashFunction);

    handleAll(-50, 50, true);
    ht.Print();
    handleAll(-50, 50, false);
    ht.Print();

    int n = 20;

    std::vector<std::thread> threads;

    for(int i = 0; i < n; i++){
        threads.push_back(std::thread(handleRandoms, thread_arg_t(0, 20, 0)));
    }

    for(int i = 0; i < n; i++){
        threads[i].join();
    }

    ht.Print();

    return 0;
}