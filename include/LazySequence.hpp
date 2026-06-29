#pragma once

#include <functional>
#include "sequence.hpp"
#include "ArraySequence.hpp"
#include "exceptions.hpp"

class Cardinal{
private:
    bool isInfinite;
    size_t value;

public:
    Cardinal();
    Cardinal(size_t val);

    bool IsInfinite() const;
    size_t GetValue() const;

    bool operator==(const Cardinal& other) const{
        if (isInfinite == other.IsInfinite()){
            return true;
        }
        if (isInfinite != other.IsInfinite()){
            return false;
        }
        return value == other.GetValue();
    }
};

template <typename T>
class IEnumerator{
public:
    virtual bool MoveNext() = 0;
    virtual T Current() = 0;
    virtual void Reset() = 0;
    virtual ~IEnumerator() = default;
};

template <typename T>
class LazySequence{
private:
    ArraySequence<T> cache;
    std::function<T(Sequence<T>*)> generator;
    Cardinal length;

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

    IEnumerator<T>* GetEnumerator();
};

inline Cardinal::Cardinal() : isInfinite(true), value(0) {}

inline Cardinal::Cardinal(size_t val) : isInfinite(false), value(val) {}

inline bool Cardinal::IsInfinite() const{
    return isInfinite;
}

inline size_t Cardinal::GetValue() const{
    return value;
}

template <typename T>
void LazySequence<T>::MaterializeUpTo(size_t index){
    if (index < cache.GetLength()){
        return;
    }

    if (!length.IsInfinite()){
        throw IndexOutOfRangeException("Индекс вне диапазона");
    }

    while (cache.GetLength() <= index){
        cache.Append(generator(&cache));
    }
}

template <typename T>
LazySequence<T>::LazySequence()
    : length(0) {}

template <typename T>
LazySequence<T>::LazySequence(T* items, size_t count)
    : length(count){
    if (items == nullptr){
        throw NullPtrException("Null указатель");
    }

    for (size_t i = 0; i < count; i++){
        cache.Append(items[i]);
    }
}

template <typename T>
LazySequence<T>::LazySequence(Sequence<T>* sequence)
    : length(sequence->GetLength()){
    if (sequence == nullptr){
        throw NullPtrException("Null указатель");
    }

    for (size_t i = 0; i < sequence->GetLength(); i++){
        cache.Append(sequence->Get(i));
    }
}

template <typename T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : cache(other.cache),
      generator(other.generator),
      length(other.length) {}

template <typename T>
LazySequence<T>::LazySequence(
    std::function<T(Sequence<T>*)> gen,
    Sequence<T>* initialItems)
    : generator(gen),
      length(){
    if (initialItems == nullptr){
        throw NullPtrException("Null указатель");
    }

    if (initialItems->GetLength() == 0){
        throw InvalidSizeException("Для генератора нужен хотя бы 1 элемент");
    }

    for (size_t i = 0; i < initialItems->GetLength(); i++){
        cache.Append(initialItems->Get(i));
    }
}

template <typename T>
T LazySequence<T>::operator[](size_t index){
    if (index >= cache.GetLength()){
        MaterializeUpTo(index);
    }

    return cache[index];
}

template <typename T>
T LazySequence<T>::Get(size_t index){
    if (index >= cache.GetLength()){
        MaterializeUpTo(index);
    }

    return (*this)[index];
}

template <typename T>
T LazySequence<T>::GetFirst(){
    if (cache.GetLength() == 0){
        throw EmptySequenceException("Пустой список");
    }

    return (*this)[0];
}

template <typename T>
T LazySequence<T>::GetLast(){
    if (length.IsInfinite()){
        throw Exception("Невозможно получить последний элемент бесконечной последовательности");
    }

    if (cache.GetLength() == 0){
        throw EmptySequenceException("Пустой список");
    }

    return (*this)[cache.GetLength() - 1];
}

template <typename T>
LazySequence<T>* LazySequence<T>::GetSubsequence(size_t startIndex, size_t endIndex){
    if (startIndex > endIndex){
        throw IndexOutOfRangeException("Начальный индекс больше конечного");
    }

    if (!length.IsInfinite() && endIndex >= cache.GetLength()){
        throw IndexOutOfRangeException("Конечный индекс вне диапазона");
    }

    Get(endIndex);

    LazySequence<T>* result = new LazySequence<T>();

    for (size_t i = startIndex; i <= endIndex; i++){
        result->Append(cache.Get(i));
    }

    return result;
}

template <typename T>
size_t LazySequence<T>::GetMaterializedCount() const{
    return cache.GetLength();
}

template <typename T>
Cardinal LazySequence<T>::GetLength() const{
    return length;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Append(T item){
    if (length.IsInfinite()){
        throw Exception("Невозможно добавить элемент в бесконечную последовательность");
    }

    cache.Append(item);
    length = Cardinal(cache.GetLength());

    return this;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Prepend(T item){
    if (length.IsInfinite()){
        throw Exception("Невозможно добавить элемент в бесконечную последовательность");
    }

    cache.Prepend(item);
    length = Cardinal(cache.GetLength());

    return this;
}

template <typename T>
LazySequence<T>* LazySequence<T>::InsertAt(T item, size_t index){
    if (length.IsInfinite()){
        throw Exception("Невозможно изменить бесконечную последовательность");
    }

    if (index > cache.GetLength()){
        throw IndexOutOfRangeException("Индекс вне диапазона");
    }

    cache.InsertAt(item, index);
    length = Cardinal(cache.GetLength());

    return this;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Concat(LazySequence<T>* other){
    if (other == nullptr){
        throw NullPtrException("Null указатель");
    }

    if (length.IsInfinite()){
        throw Exception("Невозможно выполнить конкатенацию бесконечной последовательности");
    }

    for (size_t i = 0; i < other->cache.GetLength(); i++){
        cache.Append(other->cache.Get(i));
    }

    length = Cardinal(cache.GetLength());

    return this;
}

template <typename T>
template <typename U>
LazySequence<U>* LazySequence<T>::Map(U (*func)(T), size_t count){
    if (func == nullptr){
        throw NullPtrException("Null указатель на функцию");
    }

    LazySequence<U>* result = new LazySequence<U>();

    for (size_t i = 0; i < count; i++){
        result->Append(func(Get(i)));
    }

    return result;
}

template <typename T>
LazySequence<T>* LazySequence<T>::Where(bool (*pred)(T), size_t count){
    if (pred == nullptr){
        throw NullPtrException("Null указатель на функцию");
    }

    LazySequence<T>* result = new LazySequence<T>();

    for (size_t i = 0; i < count; i++){
        T item = Get(i);

        if (pred(item)){
            result->Append(item);
        }
    }

    return result;
}

template <typename T>
template <typename U>
U LazySequence<T>::Reduce(U (*func)(U, T), U initial, size_t count){
    if (func == nullptr){
        throw NullPtrException("Null указатель на функцию");
    }

    U result = initial;

    for (size_t i = 0; i < count; i++){
        result = func(result, Get(i));
    }

    return result;
}

template <typename T>
IEnumerator<T>* LazySequence<T>::GetEnumerator(){
    class LazyEnumerator : public IEnumerator<T>{
    private:
        LazySequence<T>* seq;
        size_t currentIndex;
        T currentValue;

    public:
        LazyEnumerator(LazySequence<T>* sequence)
            : seq(sequence), currentIndex(0) {}

        bool MoveNext() override{
            try{
                if (seq->GetLength().IsInfinite() || currentIndex < seq->GetLength().GetValue()){
                    currentValue = seq->Get(currentIndex);
                    currentIndex++;
                    return true;
                }
            } 
            catch (...){
                return false;
            }
            return false;
        }

        T Current() override{
            return currentValue;
        }

        void Reset() override{
            currentIndex = 0;
        }
    };
    return new LazyEnumerator(this);
}