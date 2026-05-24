#pragma once
#include <iostream>
#include <type_traits>
#include <string>

#include "exceptions.hpp"

using namespace std;






template <typename T>
struct SequenceStats{
    T max;
    T min;
    double avg;
};

template <typename T>
class Sequence{
public:
    virtual ~Sequence() = default;
    virtual T GetFirst()  const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(size_t index) const = 0;
    virtual Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const = 0;
    virtual size_t GetLength() const = 0; 
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, size_t index) = 0;
    virtual Sequence<T>* Set(size_t index, T item) = 0;
    virtual Sequence <T>* Concat(Sequence <T> *list) = 0;

    SequenceStats<T> get_stats() const requires is_arithmetic_v<T>; 
    virtual Sequence<T> *ReflectSum() const;
    size_t GetInversions() const;

    T operator[](size_t index){ 
        return Get(index); 
    }
};


template <typename T>
SequenceStats<T> Sequence<T>::get_stats() const requires is_arithmetic_v<T>{
    size_t len = this->GetLength();
    if (len == 0){
        throw EmptySequenceException("Список пуст");
    }

    SequenceStats<T> Stats {};
    Stats.max = this->Get(0);
    Stats.min = this->Get(0);
    Stats.avg = 0.0;

    for(size_t i = 0; i < len; i++){
        T elem = this->Get(i);
        if(elem > Stats.max){
            Stats.max = elem;
        }
        if(elem < Stats.min){
            Stats.min = elem;
        }
        Stats.avg += elem;
    }
    Stats.avg = Stats.avg / len;

    return Stats;
}

template <typename T>
Sequence<T>* Sequence<T>::ReflectSum() const{
    throw InvalidSizeException("Нет реализации");
}



template <typename T>
size_t Sequence<T>::GetInversions() const{
    size_t count = 0;
    size_t len = GetLength();
    for (size_t i = 0; i < len; ++i){
        for (size_t j = i + 1; j < len; ++j){
            if(Get(i) > Get(j)){
                count++;
            } 
        }  
    }
    return count;
}