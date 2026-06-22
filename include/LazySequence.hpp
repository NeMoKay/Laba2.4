#pragma once

#include <functional>
#include "sequence.hpp"
#include "ArraySequence.hpp"
#include "exceptions.hpp"

class Cardinal{
private:
    bool isInfinite;
    size_t value;

    Cardinal(bool isInfinite, size_t val);

public:
    Cardinal(size_t val);
    static Cardinal Infinite();

    bool IsInfinite() const;
    size_t GetValue() const;
};

template <typename T>
class LazySequence{
private:
    ArraySequence<T> cache;
    std::function<T(Sequence<T>*)> generator;
    bool isInfinite;

    void MaterializeUpTo(size_t index);

public:
    LazySequence();
    LazySequence(T* items, size_t count);
    LazySequence(Sequence<T>* sequence);
    LazySequence(const LazySequence<T>& other);
    LazySequence(std::function<T(Sequence<T>*)> gen, Sequence<T>* initialItems);

    T operator[](size_t index);
    T Get(size_t index);
    T GetFirst();
    T GetLast();
    LazySequence<T>* GetSubsequence(size_t startIndex, size_t endIndex);
    
    size_t GetMaterializedCount() const;
    Cardinal GetLength() const;

    LazySequence<T>* Append(T item);
    LazySequence<T>* Prepend(T item);
    LazySequence<T>* InsertAt(T item, size_t index);
    LazySequence<T>* Concat(LazySequence<T>* other);

    template <typename U>
    LazySequence<U>* Map(U (*func)(T), size_t count);

    LazySequence<T>* Where(bool (*pred)(T), size_t count);

    template <typename U>
    U Reduce(U (*func)(U, T), U initial, size_t count);
};

inline Cardinal::Cardinal(bool isInfinite, size_t val) : isInfinite(isInfinite), value(val)  {}

inline Cardinal::Cardinal(size_t val) : Cardinal(false, val)  {}

inline Cardinal Cardinal::Infinite(){ 
    return Cardinal(true, 0); 
}

inline bool Cardinal::IsInfinite() const{ 
    return isInfinite; 
}

inline size_t Cardinal::GetValue() const{ 
    return value; 
}

template <typename T>
void LazySequence<T>::MaterializeUpTo(size_t index){
    if(index < cache.GetLength()){ return; }

    if(!isInfinite){
        throw IndexOutOfRangeException("Индекс вне диапазона");
    }

    while(cache.GetLength() <= index){
        cache.Append(generator(&cache));
    }
}

template <typename T>
LazySequence<T>::LazySequence() : isInfinite(false)  {}

template <typename T>
LazySequence<T>::LazySequence(T* items, size_t count) : isInfinite(false){
    if(items == nullptr){
        throw NullPtrException("Null указатель");
    }
    for(size_t i = 0; i < count; i++){ 
        cache.Append(items[i]); 
    }
}

template <typename T>
LazySequence<T>::LazySequence(Sequence<T>* sequence) : isInfinite(false){
    if(sequence == nullptr){
        throw NullPtrException("Null указатель");
    }
    for(size_t i = 0; i < sequence->GetLength(); i++){
        cache.Append(sequence->Get(i));
    }
}

template <typename T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : cache(other.cache), generator(other.generator), isInfinite(other.isInfinite)  {}

template <typename T>
LazySequence<T>::LazySequence(std::function<T(Sequence<T>*)> gen, Sequence<T>* initialItems)
    : generator(gen), isInfinite(true){
    if(initialItems == nullptr){
        throw NullPtrException("Null указатель");
    }
    if(initialItems->GetLength() == 0){
        throw InvalidSizeException("Для генератора нужен хотя бы 1 элемент");
    }
    for(size_t i = 0; i < initialItems->GetLength(); i++){ 
        cache.Append(initialItems->Get(i)); 
    }
}

template <typename T>
T LazySequence<T>::operator[](size_t index){
    if(index >= cache.GetLength()){ MaterializeUpTo(index); }
    return cache[index];
}

template <typename T>
T LazySequence<T>::Get(size_t index){
    if(index >= cache.GetLength()){ MaterializeUpTo(index); }
    return (*this)[index];
}

template <typename T>
T LazySequence<T>::GetFirst(){
    if(cache.GetLength() == 0){ throw EmptySequenceException("Пустой список"); }
    return (*this)[0];
}

template <typename T>
T LazySequence<T>::GetLast(){
    if(isInfinite){
        throw Exception("Невозможно получить последний элемент бесконечной последовательности");
    }
    if(cache.GetLength() == 0){ throw EmptySequenceException("Пустой список"); }
    return (*this)[cache.GetLength() - 1];
}

template <typename T>
LazySequence<T>* LazySequence<T>::GetSubsequence(size_t startIndex, size_t endIndex){
    if(startIndex > endIndex){
        throw IndexOutOfRangeException("Начальный индекс больше конечного");
    }
    if(!isInfinite && endIndex >= cache.GetLength()){
        throw IndexOutOfRangeException("Конечный индекс вне диапазона");
    }
    Get(endIndex);
    LazySequence<T>* result = new LazySequence<T>();
    for(size_t i = startIndex; i <= endIndex; i++){
        result->cache.Append(this->cache.Get(i));
    }
    return result;
}

template <typename T>
size_t LazySequence<T>::GetMaterializedCount() const{ 
    return cache.GetLength(); 
}

template <typename T>
Cardinal LazySequence<T>::GetLength() const{
    if(isInfinite){ return Cardinal::Infinite(); }
    return Cardinal(cache.GetLength());
}

template <typename T>
LazySequence<T>* LazySequence<T>::Append(T item){
    if(isInfinite){
        throw Exception("Невозможно добавить элемент в бесконечную последовательность");
    }
    cache.Append(item);
    return this;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Prepend(T item){
    cache.Prepend(item);
    return this;
}

template <typename T>
LazySequence<T>* LazySequence<T>::InsertAt(T item, size_t index){
    if(index > cache.GetLength()){
        throw IndexOutOfRangeException("Индекс вне диапазона");
    }
    cache.InsertAt(item, index);
    return this;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Concat(LazySequence<T>* other){
    if(other == nullptr){ throw NullPtrException("Null указатель"); }
    if(isInfinite){
        throw Exception("Невозможно выполнить конкатенацию бесконечной последовательности");
    }
    for(size_t i = 0; i < other->cache.GetLength(); i++){
        cache.Append(other->cache.Get(i));
    }
    return this;
}

template <typename T>
template <typename U>
LazySequence<U>* LazySequence<T>::Map(U (*func)(T), size_t count){
    if(func == nullptr){
        throw NullPtrException("Null указатель на функцию");
    }
    LazySequence<U>* result = new LazySequence<U>();
    for(size_t i = 0; i < count; i++){
        result->Append(func(this->Get(i)));
    }
    return result;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Where(bool (*pred)(T), size_t count){
    if(pred == nullptr){
        throw NullPtrException("Null указатель на функцию");
    }
    LazySequence<T>* result = new LazySequence<T>();
    for(size_t i = 0; i < count; i++){
        T item = this->Get(i);
        if(pred(item)){ result->cache.Append(item); }
    }
    return result;
}

template <typename T>
template <typename U>
U LazySequence<T>::Reduce(U (*func)(U, T), U initial, size_t count){
    if(func == nullptr){
        throw NullPtrException("Null указатель на функцию");
    }
    U result = initial;
    for(size_t i = 0; i < count; i++){
        result = func(result, this->Get(i));
    }
    return result;
}