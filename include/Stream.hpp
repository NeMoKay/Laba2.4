#pragma once

#include "sequence.hpp"
#include "LazySequence.hpp"
#include "exceptions.hpp"

template <typename T>
class ReadOnlyStream{
private:
    Sequence<T>* seqSource;
    LazySequence<T>* lazySource;
    size_t position;
    bool isFinite;
    bool isLazy;

public:
    ReadOnlyStream(Sequence<T>* seq);
    ReadOnlyStream(LazySequence<T>* lazySeq);

    bool IsEndOfStream() const;
    T Read();
    size_t GetPosition() const;
    bool IsCanSeek() const;
    size_t Seek(size_t index);
    bool IsCanGoBack() const;
    void Open();
    void Close();
};

template <typename T>
class WriteOnlyStream{
private:
    LazySequence<T>& target;
    size_t position;

public:
    WriteOnlyStream(LazySequence<T>& lazySeq);

    size_t GetPosition() const;
    size_t Write(T item);
    void Open();
    void Close();
};


template <typename T>
ReadOnlyStream<T>::ReadOnlyStream(Sequence<T>* seq)
: seqSource(seq), lazySource(nullptr), position(0), isFinite(true), isLazy(false) {}

template <typename T>
ReadOnlyStream<T>::ReadOnlyStream(LazySequence<T>* lazySeq)
: seqSource(nullptr), lazySource(lazySeq), position(0), isLazy(true){
    isFinite = !lazySeq->GetLength().IsInfinite();
}

template <typename T>
bool ReadOnlyStream<T>::IsEndOfStream() const{
    if(!isFinite){ 
        return false; 
    }
    if(isLazy){
        return position >= lazySource->GetMaterializedCount();
    } 
    else{
        return position >= seqSource->GetLength();
    }
}

template <typename T>
T ReadOnlyStream<T>::Read(){
    if(IsEndOfStream()){ 
        throw Exception("Достигнут конец потока"); 
    }
    T item;
    if(isLazy){
        item = lazySource->Get(position);
    } 
    else{
        item = seqSource->Get(position);
    }
    position++;
    return item;
}

template <typename T>
size_t ReadOnlyStream<T>::GetPosition() const{ 
    return position; 
}

template <typename T>
bool ReadOnlyStream<T>::IsCanSeek() const{ 
    return true; 
}

template <typename T>
size_t ReadOnlyStream<T>::Seek(size_t index){
    if(isFinite){
        size_t maxLen = isLazy ? lazySource->GetMaterializedCount() : seqSource->GetLength();
        if(index >= maxLen){
            throw IndexOutOfRangeException("Ошибка индекса потока");
        }
    }
    position = index;
    return position;
}

template <typename T>
bool ReadOnlyStream<T>::IsCanGoBack() const{ 
    return true; 
}

template <typename T>
void ReadOnlyStream<T>::Open() {}

template <typename T>
void ReadOnlyStream<T>::Close(){ 
    position = 0; 
}


template <typename T>
WriteOnlyStream<T>::WriteOnlyStream(LazySequence<T>& lazySeq)
: target(lazySeq), position(0) {}

template <typename T>
size_t WriteOnlyStream<T>::GetPosition() const{ 
    return position; 
}

template <typename T>
size_t WriteOnlyStream<T>::Write(T item){
    target.Append(item);
    position++;
    return position;
}

template <typename T>
void WriteOnlyStream<T>::Open() {}

template <typename T>
void WriteOnlyStream<T>::Close(){ 
    position = 0; 
}