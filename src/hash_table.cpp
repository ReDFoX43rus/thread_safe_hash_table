#include <iostream>
#include "hash_table.h"

template<class T>
CHashTable<T>::CHashTable(){
    for(int i = 0; i < MAIN_ARR_SIZE; i++)
        m_apEntities[i] = nullptr;

    Hash = nullptr;
}

template<class T>
CHashTable<T>::~CHashTable(){
    m_EntitiesMtx.lock();

    for(int i = 0; i < MAIN_ARR_SIZE; i++){
        if(m_apEntities[i] != nullptr)
            DestroyList(m_apEntities[i]);
    }

    m_EntitiesMtx.unlock();
}

template<class T>
void CHashTable<T>::DestroyList(node_t<T> *node){
    if(node == nullptr)
        return;

    node_t<T> *next = node->next;

    delete node;
    DestroyList(next);
}

template<class T>
void CHashTable<T>::SetHashFunction(int (*Hash)(T value)){
    this->Hash = Hash;
}

template<class T>
int CHashTable<T>::GetHash(T value){
    return Hash == nullptr ? 0 : Hash(value) % MAIN_ARR_SIZE;
}

template<class T>
bool CHashTable<T>::Add(T value){
    node_t<T> *parent;
    node_t<T> *result = InnerFind(value, &parent);

    if(result != nullptr){
        if(parent != nullptr){
            parent->mtx.unlock();
        }

        return false;
    }

    if(parent == nullptr){
        int hash = GetHash(value);
        m_apEntities[hash] = new node_t<T> (value, nullptr);
        m_EntitiesMtx.unlock();

        return true;
    }

    parent->next = new node_t<T> (value, nullptr);
    parent->mtx.unlock();
    return true;
}

template<class T>
bool CHashTable<T>::Remove(T value){
    node_t<T> *parent;
    node_t<T> *to_remove = InnerFind(value, &parent);

    if(to_remove == nullptr){
        if(parent == nullptr)
            m_EntitiesMtx.unlock();
        else parent->mtx.unlock();

        return false;
    }

    if(parent == nullptr){
        T value = to_remove->value;
        int hash = GetHash(value);

        to_remove->mtx.unlock();

        m_EntitiesMtx.lock();

        to_remove->mtx.lock();
        to_remove->mtx.unlock();

        m_apEntities[hash] = to_remove->next;

        delete to_remove;

        m_EntitiesMtx.unlock();

        return true;
    }

    to_remove->mtx.unlock();
    parent->next = to_remove->next;

    delete to_remove;

    parent->mtx.unlock();

    return true;
}

template<class T>
bool CHashTable<T>::Find(T value){
    node_t<T> *parent;
    node_t<T> *result = InnerFind(value, &parent);

    if(result == nullptr){
        if(parent == nullptr){
            m_EntitiesMtx.unlock();
        } else {
            parent->mtx.unlock();
        }

        return false;        
    } else {
        if(parent != nullptr){
            parent->mtx.unlock();
        }

        return true;
    }

}

template<class T>
node_t<T> *CHashTable<T>::InnerFind(T value, node_t<T> **parent){
    *parent = nullptr;

    int hash = GetHash(value);

    m_EntitiesMtx.lock();
    if(m_apEntities[hash] == nullptr){
        // need to unlock m_EntitiesMtx
        return nullptr;
    }

    node_t<T> *current = m_apEntities[hash];

    current->mtx.lock();
    m_EntitiesMtx.unlock();

    node_t<T> *next;

    while(current){
        if(current->value == value){
            // need to unlock current->mtx and *parent->mtx if not nullptr
            return current;
        }

        next = current->next;
        if(next == nullptr)
            break;

        next->mtx.lock();

        if(parent != nullptr)
            (*parent)->mtx.unlock();

        *parent = current;
        current = next;
    }

    (*parent)->mtx.unlock();
    *parent = current;
    return nullptr;
}

template<class T>
void CHashTable<T>::Print(){
    m_EntitiesMtx.lock();

    for(int i = 0; i < MAIN_ARR_SIZE; i++){
        cout << "#" << i << ": ";
        PrintList(m_apEntities[i]);
        cout << endl;
    }

    m_EntitiesMtx.unlock();
}

template<class T>
void CHashTable<T>::PrintList(node_t<T> *node){
    if(node == nullptr)
        return;

    cout << node->value << " ";
    PrintList(node->next);
}