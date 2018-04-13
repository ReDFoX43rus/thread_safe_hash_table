#include "hash_table.h"

using IntTable = CHashTable<int>;

static int HashFunction(int value){
    return (value*2654435761) & 0xFF;
}

int main(){
    IntTable ht;

    ht.SetHashFunction(&HashFunction);

    for(int i = 0; i < 10000; i += 3)
        ht.Add(i);

    ht.Print();

    for(int i = 0; i < 10000; i += 6)
        ht.Remove(i);

    ht.Print();

    return 0;
}