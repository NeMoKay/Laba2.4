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
    ReadOnlyStream(Sequence<T>* seq)
    : seqSource(seq), lazySource(nullptr), position(0), isFinite(true), isLazy(false)  {}

    ReadOnlyStream(LazySequence<T>* lazySeq)
    : seqSource(nullptr), lazySource(lazySeq), position(0), isLazy(true){
        isFinite = !lazySeq->GetLength().IsInfinite();
    }

    bool IsEndOfStream() const{
        if(!isFinite){ return false; }
        if(isLazy){
            return position >= lazySource->GetMaterializedCount();
        } 
        else{
            return position >= seqSource->GetLength();
        }
    }

    T Read(){
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

    size_t GetPosition() const{ 
        return position; 
    }

    bool IsCanSeek() const{ 
        return true; 
    }

    size_t Seek(size_t index){
        if(isFinite){
            size_t maxLen = isLazy ? lazySource->GetMaterializedCount() : seqSource->GetLength();
            if(index >= maxLen){
                throw IndexOutOfRangeException("Ошибка индекса потока");
            }
        }
        position = index;
        return position;
    }

    bool IsCanGoBack() const{ 
        return true; 
    }

    void Open()  {}
    void Close(){ position = 0; }
};

template <typename T>
class WriteOnlyStream{
private:
    LazySequence<T>& target;
    size_t position;

public:
    WriteOnlyStream(LazySequence<T>& lazySeq)
    : target(lazySeq), position(0)  {}

    size_t GetPosition() const{ 
        return position; 
    }

    size_t Write(T item){
        target.Append(item);
        position++;
        return position;
    }

    void Open()  {}
    void Close(){ position = 0; }
};