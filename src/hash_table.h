#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define MAIN_ARR_SIZE 255

#include <mutex>
using namespace std;

template<class T>
struct node_t{

    T value;

    node_t<T> *next;
    mutex mtx;

    node_t(T value, node_t<T>* next){
        this->value = value;
        this->next = next;
    }

    node_t(){
        value = NULL;
        next = nullptr;
    }
};

template<class T>
class CHashTable{
public:
    CHashTable();
    ~CHashTable();

    void SetHashFunction(int (*Hash)(T value));

    bool Find(T value);
    bool Add(T value);
    bool Remove(T value);

    void Print();
private:
    mutex m_EntitiesMtx;
    node_t<T> *m_apEntities[MAIN_ARR_SIZE];

    int (*Hash)(T value);
    int GetHash(T value);

    node_t<T> *InnerFind(T value, node_t<T> **parent);

    void PrintList(node_t<T> *node);
    void DestroyList(node_t<T> *node);
};

template class CHashTable<int>;

#endif