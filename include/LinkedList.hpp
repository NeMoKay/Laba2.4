#pragma once

#include <iostream>
#include <cstddef>
#include <string>
#include <format>
#include "exceptions.hpp"

template <typename T > 
class LinkedList{
private:
    struct Node{
        T value;
        Node *next;
        Node *prev;

        Node(T new_value, Node *new_next, Node *new_prev);
        Node();
        Node(T val);
    };
    
    Node *head;
    Node *tail;
    
public:
    template <size_t N>
    LinkedList(T (&arr)[N]);
    
    LinkedList();
    LinkedList(const LinkedList<T>& list);

    T GetFirst() const;
    T GetLast() const;
    T Get(size_t index) const;
    LinkedList<T>* GetSubList(size_t startIndex, size_t endIndex) const;
    size_t GetLength() const;

    void Append(T item);
    void Prepend(T item);
    void InsertAt(T item, size_t index);
    void Set(size_t index, T item);
    LinkedList<T>* Concat(LinkedList<T> *list);

    ~LinkedList();

    class Iterator{
    private:
        Node* current;
    public:
        Iterator(Node* node);
        T operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;
    Iterator end() const;
};

//private Node
template <typename T >
LinkedList<T>::Node::Node(T new_value, Node *new_next, Node *new_prev){
    value = new_value;
    next = new_next;
    prev = new_prev;
}

template <typename T >
LinkedList<T>::Node::Node(){
    next = nullptr;
    prev = nullptr;
}

template <typename T >
LinkedList<T>::Node::Node(T val){
    next = nullptr;
    prev = nullptr;
    value = val;
}

//public
template <typename T>
template <size_t N>
LinkedList<T>::LinkedList(T (&arr)[N]){
    if(N == 0){
        throw InvalidSizeException("Размер <= 0 ");
    }

    head = nullptr;
    tail = nullptr;
    Node *prev_elem = nullptr;
    Node *now_elem = nullptr;

    for(size_t i = 0; i < N; i++){
        now_elem = new Node;

        now_elem->value = arr[i];
        now_elem->prev = prev_elem;
        if(i != 0){
            prev_elem->next = now_elem;
        }

        prev_elem = now_elem;
        if(i == 0){
            head = now_elem;
        }
        if(i == N - 1){
            tail = now_elem;
        }
    }
}

template <typename T >
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr)  {}

template <typename T >
LinkedList<T>::LinkedList(const LinkedList<T>& list) : head(nullptr), tail(nullptr){
    Node* now_elem = list.head;

    while(now_elem != nullptr){
        Append(now_elem->value);
        now_elem = now_elem->next;
    }
}

template <typename T >
T LinkedList<T>::GetFirst() const{
    if(head == nullptr){
        throw EmptySequenceException("Список пуст");
    }
    return head->value;
}

template <typename T >
T LinkedList<T>::GetLast() const{
    if(tail == nullptr){
        throw EmptySequenceException("Список пуст");
    }
    return tail->value;
}

template <typename T >
T LinkedList<T>::Get(size_t index) const{
    size_t length = GetLength();
    if(index >= length){
        throw IndexOutOfRangeException(std::format("Индекс вне списка (индекс:  {}, размер:  {})", index, length));
    }

    if(index < length / 2){
        Node *now_elem = head;
        for(size_t i = 0; i < index; i++){
            now_elem = now_elem->next;
        }
        return now_elem->value;
    }
    else{
        Node *now_elem = tail;
        for(size_t i = 0; i < length - index - 1; i++){
            now_elem = now_elem->prev;
        }
        return now_elem->value;
    }
}

template <typename T >
void LinkedList<T>::Set(size_t index, T item){
    size_t length = GetLength();
    if(index >= length){
        throw IndexOutOfRangeException(std::format("Индекс вне списка (индекс:  {}, размер:  {})", index, length));
    }

    if(index < length / 2){
        Node *now_elem = head;
        for(size_t i = 0; i < index; i++){
            now_elem = now_elem->next;
        }
        now_elem->value = item;
    }
    else{
        Node *now_elem = tail;
        for(size_t i = 0; i < length - index - 1; i++){
            now_elem = now_elem->prev;
        }
        now_elem->value = item;
    }
}

template <typename T >
LinkedList<T>* LinkedList<T>::GetSubList(size_t startIndex, size_t endIndex) const{
    size_t length = GetLength();
    if(endIndex < startIndex || startIndex >= length || endIndex >= length){
        throw IndexOutOfRangeException(std::format("Ошибка индекса (start:  {}, end:  {}, size:  {})", startIndex, endIndex, length));
    }

    LinkedList<T>* result = new LinkedList<T>();

    Node *now_elem = head;
    size_t current_pos = 0;
    while(now_elem != nullptr){
        if(current_pos >= startIndex && current_pos <= endIndex){
            result->Append(now_elem->value);
        }
        now_elem = now_elem->next;
        current_pos++;
    }

    return result;
}

template <typename T >
size_t LinkedList<T>::GetLength() const{
    size_t count = 0;
    Node *now_elem = head;
    while(now_elem != nullptr){
        count++;
        now_elem = now_elem->next;
    }
    return count;
}

template <typename T >
void LinkedList<T>::Append(T item){
    Node *new_elem = new Node;
    new_elem->value = item;
    
    if(head != nullptr){
        new_elem->prev = tail;
        tail->next = new_elem;
        tail = new_elem;
    }
    else{
        head = new_elem;
        tail = new_elem;
    }
}

template <typename T >
void LinkedList<T>::Prepend(T item){
    Node *new_elem = new Node;
    new_elem->value = item;

    if(head != nullptr){
        new_elem->next = head;
        head->prev = new_elem;
        head = new_elem;
    }
    else{
        head = new_elem;
        tail = new_elem;
    }
}

template <typename T >
void LinkedList<T>::InsertAt(T item, size_t index){
    size_t length = GetLength();
    if(index > length){
        throw IndexOutOfRangeException(std::format("Индекс вне диапазона (индекс:  {}, максимум:  {})", index, length));
    }

    if(index == 0){
        this->Prepend(item);
        return;
    }
    if(index == length){
        this->Append(item);
        return;
    }

    Node *now_elem = head;
    for(size_t i = 0; i < index; i++){
        now_elem = now_elem->next;
    }

    Node *new_elem = new Node;
    new_elem->value = item;
    new_elem->prev = now_elem->prev;
    new_elem->next = now_elem;
    (now_elem->prev)->next = new_elem;
    now_elem->prev = new_elem;
}

template <typename T>
LinkedList<T>* LinkedList<T>::Concat(LinkedList<T> *list){
    if(list == nullptr){
        return this;
    }
    
    size_t other_length = list->GetLength();
    for(size_t i = 0; i < other_length; i++){
        this->Append(list->Get(i));
    }
    return this;
}

template <typename T >
LinkedList<T>::~LinkedList(){
    Node *now_elem = head;
    while(now_elem != nullptr){
        Node *next_elem = now_elem->next;
        delete now_elem;
        now_elem = next_elem;
    }
    head = nullptr;
    tail = nullptr;
}

template <typename T>
LinkedList<T>::Iterator::Iterator(Node* node) : current(node)  {}

template <typename T>
T LinkedList<T>::Iterator::operator*() const{ 
    return current->value; 
}

template <typename T>
typename LinkedList<T>::Iterator& LinkedList<T>::Iterator::operator++(){ 
    if (current != nullptr){
        current = current->next;
    }
    return *this; 
}

template <typename T>
bool LinkedList<T>::Iterator::operator!=(const Iterator& other) const{ 
    return current != other.current; 
}

template <typename T>
typename LinkedList<T>::Iterator LinkedList<T>::begin() const{ 
    return Iterator(head); 
}

template <typename T>
typename LinkedList<T>::Iterator LinkedList<T>::end() const{ 
    return Iterator(nullptr); 
}