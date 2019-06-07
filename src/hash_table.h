#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define MAIN_ARR_SIZE 256

#include <mutex>
#include <iostream>
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
    CHashTable(){
		for(int i = 0; i < MAIN_ARR_SIZE; i++)
        m_apEntities[i] = nullptr;

		Hash = nullptr;
	}
    ~CHashTable(){
		m_EntitiesMtx.lock();

		for(int i = 0; i < MAIN_ARR_SIZE; i++){
			if(m_apEntities[i] != nullptr)
				DestroyList(m_apEntities[i]);
		}

		m_EntitiesMtx.unlock();
	}

    void SetHashFunction(int (*Hash)(T value)){
		this->Hash = Hash;
	}

    bool Find(T value){
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

			result->mtx.unlock();
			return true;
		}
	}
    bool Add(T value){
		node_t<T> *parent;
		node_t<T> *result = InnerFind(value, &parent);

		if(result != nullptr){
			if(parent != nullptr){
				parent->mtx.unlock();
			}

			result->mtx.unlock();

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
    bool Remove(T value){
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

    void Print(){
		m_EntitiesMtx.lock();

		for(int i = 0; i < MAIN_ARR_SIZE; i++){
			cout << "#" << i << ": ";
			PrintList(m_apEntities[i]);
			cout << endl;
		}

		m_EntitiesMtx.unlock();
	}
private:
    mutex m_EntitiesMtx;
    node_t<T> *m_apEntities[MAIN_ARR_SIZE];

    int (*Hash)(T value);
    int GetHash(T value) {
		return Hash == nullptr ? 0 : Hash(value) % MAIN_ARR_SIZE;
	}

    node_t<T> *InnerFind(T value, node_t<T> **parent){
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

			if(*parent != nullptr)
				(*parent)->mtx.unlock();

			*parent = current;
			current = next;
		}

		if(*parent != nullptr)
			(*parent)->mtx.unlock();

		*parent = current;
		return nullptr;
	}

    void PrintList(node_t<T> *node){
		if(node == nullptr)
			return;

		cout << node->value << " ";
		PrintList(node->next);
	}
    void DestroyList(node_t<T> *node){
		if(node == nullptr)
        return;

		node_t<T> *next = node->next;

		delete node;
		DestroyList(next);
	}
};

//template class CHashTable<int>;

#endif