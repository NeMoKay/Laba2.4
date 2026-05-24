#pragma once

#include "sequence.hpp"
#include <iostream>
#include <cstddef>
#include <string>

#include "exceptions.hpp"



 


template <typename T>
class DynamicArray{
private:
    T* data;
    size_t size;
public:
    DynamicArray();
    DynamicArray(size_t count);
    DynamicArray(T* items, size_t count);
    DynamicArray(const DynamicArray<T> & array);

    T Get(size_t index);
    void Set(size_t index, T value);
    void Resize(size_t newSize);
    size_t GetSize();
    T operator[](size_t index);

    ~DynamicArray();
};


//public
template <typename T >
DynamicArray<T>::DynamicArray() : data(nullptr), size(0) {}

template <typename T >
DynamicArray<T>::DynamicArray(size_t count) : data(new T[count]), size(count) {}

template <typename T >
DynamicArray<T>::DynamicArray(T* items, size_t count) : DynamicArray(count){
    if(items == nullptr){
        throw NullPtrException("Переданный массив пуст");
    }
    for(size_t i = 0; i < size; i++){
        data[i] = items[i];
    }
}

template <typename T >
DynamicArray<T>::DynamicArray(const DynamicArray<T> & array) : size(array.size){
    data = new T[size];
    for(size_t i = 0; i < size; i++){
        data[i] = array.data[i];
    }
}

template <typename T >
T DynamicArray<T>::Get(size_t index){
    if(index >= size){
        throw IndexOutOfRangeException("Ошибка индекса");
    }
    return data[index];
}

template <typename T >
void DynamicArray<T>::Set(size_t index, T value){
    if(index >= size){
        throw IndexOutOfRangeException("Ошибка индекса");
    }
    data[index] = value;
}

template <typename T >
void DynamicArray<T>::Resize(size_t newSize){
    T *new_data = new T[newSize];
    if(size != 0){
        for(size_t i = 0; i < newSize; i++){
            if(i == size-1){
                new_data[i] = data[i];
                break;
            }
            new_data[i] = data[i];
        }
    }
    
    delete[] data;
    data = new_data;
    size = newSize;
}

template <typename T >
size_t DynamicArray<T>::GetSize(){
    return size;
}


template <typename T >
T DynamicArray<T>::operator[](size_t index){
    return Get(index); 
}


template <typename T >
DynamicArray<T>::~DynamicArray(){
    delete[] data;
}