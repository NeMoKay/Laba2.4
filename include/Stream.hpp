#pragma once

#include "sequence.hpp"
#include "LazySequence.hpp"
#include "exceptions.hpp"

template <typename T>
class ReadOnlyStream{
private:
    Sequence<T>* seq;
    LazySequence<T>* lazyseq;
    size_t position;
    bool isFinite;
    bool isLazy;

public:
    ReadOnlyStream(Sequence<T>* seq);
    ReadOnlyStream(LazySequence<T>* lazySeq);

    bool IsEndOfStream() const;
    T Read();
    size_t GetPosition() const;
    size_t Seek(size_t index);
};

template <typename T>
class WriteOnlyStream{
private:
    LazySequence<T>* target;
    size_t position;

public:
    WriteOnlyStream(LazySequence<T>* lazySeq);

    size_t GetPosition() const;
    size_t Write(T item);
};


template <typename T>
ReadOnlyStream<T>::ReadOnlyStream(Sequence<T>* seq)
: seq(seq), lazyseq(nullptr), position(0), isFinite(true), isLazy(false) {}

template <typename T>
ReadOnlyStream<T>::ReadOnlyStream(LazySequence<T>* lazySeq)
: seq(nullptr), lazyseq(lazySeq), position(0), isLazy(true){
    isFinite = !lazySeq->GetLength().IsInfinite();
}

template <typename T>
bool ReadOnlyStream<T>::IsEndOfStream() const{
    if(!isFinite){ 
        return false; 
    }
    if(isLazy){
        return position >= lazyseq->GetMaterializedCount();
    } 
    else{
        return position >= seq->GetLength();
    }
}

template <typename T>
T ReadOnlyStream<T>::Read(){
    if(IsEndOfStream()){ 
        throw Exception("Достигнут конец потока"); 
    }
    T item;
    if(isLazy){
        item = lazyseq->Get(position);
    } 
    else{
        item = seq->Get(position);
    }
    position++;
    return item;
}

template <typename T>
size_t ReadOnlyStream<T>::GetPosition() const{ 
    return position; 
}

template <typename T>
size_t ReadOnlyStream<T>::Seek(size_t index){
    if(isFinite){
        size_t maxLen;
        if (isLazy){
            maxLen = lazyseq->GetMaterializedCount();
        } 
        else{
            maxLen = seq->GetLength();
        }
        
        if(index >= maxLen){
            throw IndexOutOfRangeException("Ошибка индекса потока");
        }
    }
    position = index;
    return position;
}

template <typename T>
WriteOnlyStream<T>::WriteOnlyStream(LazySequence<T>* lazySeq)
: target(lazySeq), position(0){
    if (target == nullptr){
        throw NullPtrException("Передан null-указатель в WriteOnlyStream");
    }
}

template <typename T>
size_t WriteOnlyStream<T>::GetPosition() const{ 
    return position; 
}

template <typename T>
size_t WriteOnlyStream<T>::Write(T item){
    target->Append(item);
    position++;
    return position;
}