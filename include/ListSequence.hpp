#pragma once

#include "sequence.hpp"
#include "LinkedList.hpp"
#include "ArraySequence.hpp"
#include "iostream"
#include <cstddef>
#include <string>

#include "exceptions.hpp"

 

template <typename T >
class ListSequence : public Sequence<T>{
private:
    LinkedList<T>* items;

protected:
    virtual ListSequence<T>* Clone() const;
    virtual ListSequence<T>* Instance();

    void AppendInternal(T item);
    void PrependInternal(T item);
    void InsertAtInternal(T item, size_t index);
    void SetInternal(size_t index, T item);
    void ConcatInternal(Sequence<T>* list);

public:
    ListSequence();
    ListSequence(T* new_items, size_t count);
    ListSequence(const ListSequence<T>& list);
    ListSequence(const ArraySequence<T>& arraySeq);

    T GetFirst() const override;
    T GetLast() const override;
    T Get(size_t index) const override;

    Sequence<T> *ReflectSum() const override;

    ListSequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const override;

    size_t GetLength() const override;

    ListSequence<T>* Append(T item) override;
    ListSequence<T>* Prepend(T item) override;
    ListSequence<T>* InsertAt(T item, size_t index) override;
    ListSequence<T>* Set(size_t index, T item) override;
    Sequence<T>* Concat(Sequence<T>* list_p) override;

    ~ListSequence();
};


// protected
template <typename T >
ListSequence<T>* ListSequence<T>::Clone() const{
    return new ListSequence<T>(*this);
}

template <typename T >
ListSequence<T>* ListSequence<T>::Instance(){
    return this;
}

template <typename T >
void ListSequence<T>::AppendInternal(T item){
    items->Append(item);
}

template <typename T >
void ListSequence<T>::PrependInternal(T item){
    items->Prepend(item);
}

template <typename T >
void ListSequence<T>::InsertAtInternal(T item, size_t index){
    if(index > items->GetLength()){
        throw IndexOutOfRangeException(std::format("Индекс вне диапазона (индекс: {}, максимум: {})", index, items->GetLength()));
    }
    items->InsertAt(item, index);
}

template <typename T >
void ListSequence<T>::ConcatInternal(Sequence<T>* list){
    if(list == nullptr){
        return;
    }

    for(size_t i = 0; i < list->GetLength(); i++){
        items->Append(list->Get(i));
    }
}



// public

template <typename T >
ListSequence<T>::ListSequence() : items(new LinkedList<T>) {}

template <typename T >
ListSequence<T>::ListSequence(T* new_items, size_t count) : items(new LinkedList<T>(new_items, count)) {}

template <typename T >
ListSequence<T>::ListSequence(const ListSequence<T>& list) : items(new LinkedList<T>(*(list.items))) {}

template <typename T >
ListSequence<T>::ListSequence(const ArraySequence<T>& arraySeq) : ListSequence(){

    size_t count = arraySeq.GetLength();

    for(size_t i = 0; i < count; ++i){
        items->Append(arraySeq.Get(i));
    }
}

template <typename T >
T ListSequence<T>::GetFirst() const{
    return items->GetFirst();
}

template <typename T >
T ListSequence<T>::GetLast() const{
    return items->GetLast();
}

template <typename T >
T ListSequence<T>::Get(size_t index) const{
    return items->Get(index);
}

template <typename T >
ListSequence<T>* ListSequence<T>::GetSubsequence(size_t startIndex, size_t endIndex) const{

    if(endIndex < startIndex || startIndex >= items->GetLength() || endIndex >= items->GetLength()){

        throw IndexOutOfRangeException(std::format("Ошибка индекса (start: {}, end: {}, size: {})", startIndex, endIndex, items->GetLength()));
    }

    LinkedList<T>* items_sub_list;
    items_sub_list = items->GetSubList(startIndex, endIndex);

    ListSequence<T>* Sub_list = new ListSequence<T>;

    for(size_t i = 0; i < items_sub_list->GetLength(); i++){
        Sub_list->Append(items_sub_list->Get(i));
    }

    return Sub_list;
}

template <typename T>
size_t ListSequence<T>::GetLength() const{
    return items->GetLength();
}

template <typename T>
ListSequence<T>* ListSequence<T>::Append(T item){
    ListSequence<T>* list = Instance();
    list->AppendInternal(item);
    return list;
}

template <typename T>
ListSequence<T>* ListSequence<T>::Prepend(T item){
    ListSequence<T>* list = Instance();
    list->PrependInternal(item);
    return list;
}

template <typename T>
ListSequence<T>* ListSequence<T>::InsertAt(T item, size_t index){

    if(index > items->GetLength()){
        throw IndexOutOfRangeException(std::format("Индекс вне диапазона (индекс: {}, максимум: {})", index, items->GetLength()));
    }

    ListSequence<T>* list = Instance();
    list->InsertAtInternal(item, index);

    return list;
}

template <typename T>
Sequence<T>* ListSequence<T>::Concat(Sequence<T>* list_p){

    ListSequence<T>* list = Instance();

    if(list_p == nullptr){
        return list;
    }

    for(size_t i = 0; i < list_p->GetLength(); i++){
        list->AppendInternal(list_p->Get(i));
    }

    return list;
}

template <typename T>
ListSequence<T>::~ListSequence(){
    delete items;
}

template <typename T>
void ListSequence<T>::SetInternal(size_t index, T item){
    items->Set(index, item);
}
template <typename T>
ListSequence<T>* ListSequence<T>::Set(size_t index, T item){
    ListSequence<T>* list = this->Instance();
    list->SetInternal(index, item);
    return list;
}




template <typename T>
class MutableListSequence : public ListSequence<T>{
protected:
    ListSequence<T>* Clone() const override{
        return new MutableListSequence<T>(*this);
    }
    ListSequence<T>* Instance() override{
        return this;
    }
    
public:
    using ListSequence<T> :: ListSequence;
};


template <typename T>
class ImmutableListSequence : public ListSequence<T>{
protected:
    ListSequence<T>* Clone() const override{
        return new ImmutableListSequence<T>(*this);
    }
    ListSequence<T>* Instance() override{
        return Clone();
    }
public:
    using ListSequence<T> :: ListSequence;
};


template <typename T>
Sequence<T>* ListSequence<T>::ReflectSum() const{
    if constexpr (is_arithmetic_v<T>){
        size_t len = this->GetLength();

        if (len == 0){
            throw EmptySequenceException("ReflectSum: список пуст");
        }
        
        T* arr = new T[len];
        for(size_t i = 0; i < len; ++i){
            arr[i] = this->Get(i) + this->Get(len - 1 - i);
        }  

        Sequence<T>* result = new ListSequence<T>(arr, len);
        delete[] arr;
        return result;
    } 
    else{
        throw InvalidSizeException("доступна только для числовых типов");
    }
}


template <typename T, typename T2>
ListSequence<T2>* Map(ListSequence<T>* seq, T2 (*func)(T)){
    ListSequence<T2>* result = new ListSequence<T2>();
    for(size_t i = 0; i < seq->GetLength(); i++){
        result->Append(func(seq->Get(i)));
    }
    return result;
}

template <typename T, typename T2>
T2 Reduce(ListSequence<T>* seq, T2 (*func)(T2, T), T2 start_val){
    T2 result = start_val;
    for(size_t i = 0; i < seq->GetLength(); i++){
        result = func(result, seq->Get(i));
    }
    return result;
}

template <typename T>
ListSequence<T>* Where(ListSequence<T>* seq, bool (*check_func)(T)){
    ListSequence<T>* result = new ListSequence<T>();
    for(size_t i = 0; i < seq->GetLength(); i++){
        T item = seq->Get(i);
        if(check_func(item)){
            result->Append(item);
        }
    }
    return result;
}