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
    
    template <size_t N>
    DynamicArray(T (&arr)[N]);
    
    DynamicArray(const DynamicArray<T> & array);

    T Get(size_t index) const;
    void Set(size_t index, T value);
    void Resize(size_t newSize);
    size_t GetSize() const;
    T operator[](size_t index) const;

    ~DynamicArray();

    class Iterator{
    private:
        T* ptr;
    public:
        Iterator(T* ptr);
        T operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;
    Iterator end() const;
};


//public
template <typename T >
DynamicArray<T>::DynamicArray() : data(nullptr), size(0)  {}

template <typename T >
DynamicArray<T>::DynamicArray(size_t count) : data(new T[count]), size(count)  {}

template <typename T>
template <size_t N>
DynamicArray<T>::DynamicArray(T (&arr)[N]) : DynamicArray(N){
    for(size_t i = 0; i < size; i++){
        data[i] = arr[i];
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
T DynamicArray<T>::Get(size_t index) const{
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
size_t DynamicArray<T>::GetSize() const{
    return size;
}


template <typename T >
T DynamicArray<T>::operator[](size_t index) const{
    return Get(index); 
}


template <typename T >
DynamicArray<T>::~DynamicArray(){
    delete[] data;
}



template <typename T>
DynamicArray<T>::Iterator::Iterator(T* ptr) : ptr(ptr)  {}

template <typename T>
T DynamicArray<T>::Iterator::operator*() const{ 
    return *ptr; 
}

template <typename T>
typename DynamicArray<T>::Iterator& DynamicArray<T>::Iterator::operator++(){ 
    ptr++; 
    return *this; 
}

template <typename T>
bool DynamicArray<T>::Iterator::operator!=(const Iterator& other) const{ 
    return ptr != other.ptr;
}

template <typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::begin() const{ 
    return Iterator(data); 
}

template <typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::end() const{ 
    return Iterator(data + size); 
}