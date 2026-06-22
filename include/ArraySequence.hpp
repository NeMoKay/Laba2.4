#pragma once

#include "sequence.hpp"
#include "Dynamic_Array.hpp"
#include "LinkedList.hpp"
#include "iostream"
#include <cstddef>
#include <string>

#include "exceptions.hpp"


template <typename T>
class ArraySequence : public Sequence<T>{
private:
    DynamicArray<T> items;
protected:
    virtual ArraySequence<T>* Clone() const;
    virtual ArraySequence<T>* Instance();

    void AppendInternal(T item);
    void PrependInternal(T item);
    void InsertAtInternal(T item, size_t index);
    void SetInternal(size_t index, T item);
    void ConcatInternal(Sequence <T> *list);
    Sequence<T>* DoReflectSum() const override;

    
public:

    template <size_t N>
    ArraySequence(T (&arr)[N]);
    
    ArraySequence();
    ArraySequence(const ArraySequence<T>& operand);

    T GetFirst() const override;
    T GetLast() const override;
    T Get(size_t index)const override;
    size_t GetLength() const override;
    ArraySequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const override;
    ArraySequence<T>* Append(T item) override;
    ArraySequence<T>* Prepend(T item) override;
    ArraySequence<T>* InsertAt(T item, size_t index) override;
    ArraySequence<T>* Set(size_t index, T item) override;
    ArraySequence<T>* Concat(Sequence<T>* list) override;

    using Iterator = typename DynamicArray<T>::Iterator;

    Iterator begin() const{return items.begin();  }
    Iterator end() const{return items.end(); }
    
    ~ArraySequence();
};


//protected
template <typename T>
ArraySequence<T>* ArraySequence<T>::Clone() const{
    ArraySequence<T>* copy = new  ArraySequence<T>(*this);
    return copy;
    
}
template <typename T>
ArraySequence<T>* ArraySequence<T>::Instance(){
    return this;
}

template <typename T >
void ArraySequence<T>::AppendInternal(T item){
    items.Resize(items.GetSize()+1);
    items.Set(items.GetSize()-1, item);
}

template <typename T >
void ArraySequence<T>::PrependInternal(T item){
    items.Resize(items.GetSize()+1);
    for(size_t i = items.GetSize() - 1; i >= 1; i--){
        items.Set(i, items.Get(i-1));
    }
    items.Set(0, item);
}

template <typename T >
void ArraySequence<T>::InsertAtInternal(T item, size_t index){
    if(index > items.GetSize()){
        throw IndexOutOfRangeException(std::format("Ошибка индекса (переданный инндекс  {} > максимальный индекс последовательности  {})",index,  items.GetSize() - 1));
    }

    if(index == 0){
        PrependInternal(item);
        return;
    }
    
    if(index == items.GetSize()){
        AppendInternal(item);
        return;
    }

    items.Resize(items.GetSize() + 1);
    for(size_t i = items.GetSize() - 1; i > index; --i){
        items.Set(i, items.Get(i - 1));
    }
    
    items.Set(index, item);
}

template <typename T >
void ArraySequence<T>::ConcatInternal(Sequence <T> *list){
    for(size_t i = 0; i < list->GetLength(); i++){
        this->AppendInternal(list->Get(i));
    }
}



//public
template <typename T>
template <size_t N>
ArraySequence<T>::ArraySequence(T (&arr)[N]) : items(arr)  {}

template <typename T >
ArraySequence<T>::ArraySequence() : items()  {}

template <typename T >
ArraySequence<T>::ArraySequence(const ArraySequence<T>& operand) : items(operand.items)  {}

template <typename T >
T ArraySequence<T>::GetFirst() const{
    if(items.GetSize() == 0){
        throw EmptySequenceException("Список пуст");
    }
    return items.Get(0);
}

template <typename T >
T ArraySequence<T>::GetLast() const{
    if(items.GetSize() == 0){
        throw EmptySequenceException("Список пуст");
    }
    return items.Get(items.GetSize() - 1);
}

template <typename T >
T ArraySequence<T>::Get(size_t index)const{
    if(index >= items.GetSize()){
        throw IndexOutOfRangeException(std::format("Ошибка индекса (индекс  {} >= размер  {})", index, items.GetSize()));
    }
    return items.Get(index);
}

template <typename T >
size_t ArraySequence<T>::GetLength() const{
    return items.GetSize();
}

template <typename T >
ArraySequence<T>* ArraySequence<T>::GetSubsequence(size_t startIndex, size_t endIndex) const{
    if(endIndex < startIndex || startIndex >= items.GetSize() || endIndex >= items.GetSize()){
        throw IndexOutOfRangeException(std::format("Ошибка индекса (start:  {}, end:  {}, size:  {})", startIndex, endIndex, items.GetSize()));
    }
    size_t len = endIndex-startIndex+1;
    ArraySequence<T>* new_arr = new ArraySequence<T>;

    for(size_t i = 0; i < len; i++){
        new_arr->Append(items.Get(startIndex + i));
    }
    return new_arr;
}

template <typename T >
ArraySequence<T>* ArraySequence<T>::Append(T item){
    ArraySequence<T>* type_Arr = this->Instance();
    type_Arr->AppendInternal(item);
    return type_Arr;
}

template <typename T >
ArraySequence<T>* ArraySequence<T>::Prepend(T item){
    ArraySequence<T>* type_Arr = this->Instance();
    type_Arr->PrependInternal(item);
    return type_Arr;
}

template <typename T >
ArraySequence<T>* ArraySequence<T>::InsertAt(T item, size_t index){
    ArraySequence<T>* arr = this->Instance();
    arr->InsertAtInternal(item, index);
    return arr;
}

template <typename T >
ArraySequence<T>* ArraySequence<T>::Concat(Sequence<T>* list){
    if(list == nullptr){
         return this;
    }
    ArraySequence<T>* type_Arr = this->Instance();
    type_Arr->ConcatInternal(list);
    return type_Arr;
}

template <typename T >
void ArraySequence<T>::SetInternal(size_t index, T item){
    items.Set(index, item); 
}

template <typename T >
ArraySequence<T>* ArraySequence<T>::Set(size_t index, T item){
    ArraySequence<T>* arr = this->Instance();
    arr->SetInternal(index, item);
    return arr;
}


template <typename T>
ArraySequence<T>::~ArraySequence()  {}

template <typename T>
class MutableArraySequence : public ArraySequence<T>{
protected:
    ArraySequence<T>* Clone() const override{
        return new MutableArraySequence<T>(*this);
    }
    ArraySequence<T>* Instance() override{
        return this;
    }
public:
    using ArraySequence<T> ::  ArraySequence;
};

template <typename T>
class ImmutableArraySequence : public ArraySequence<T>{
protected:
    ArraySequence<T>* Clone() const override{
        return new ImmutableArraySequence<T>(*this);
    }
    ArraySequence<T>* Instance() override{
        return Clone();
    }

public:
    using ArraySequence<T> :: ArraySequence;

};

template <typename T>
Sequence<T>* ArrayReflectSumImpl(const ArraySequence<T>* seq) requires std::is_arithmetic_v<T>{
    size_t len = seq->GetLength();
    ArraySequence<T>* result = new ArraySequence<T>();
    for(size_t i = 0; i < len; ++i){
        result->Append(seq->Get(i) + seq->Get(len - 1 - i));
    }  
    return result;
}

template <typename T>
Sequence<T>* ArrayReflectSumImpl(const ArraySequence<T>* seq) requires (!std::is_arithmetic_v<T>){
    return nullptr;
}

template <typename T>
Sequence<T>* ArraySequence<T>::DoReflectSum() const{
    return ArrayReflectSumImpl<T>(this);
}

template <typename T, typename T2>
ArraySequence<T2>* Map(ArraySequence<T>* seq, T2 (*func)(T)){
    ArraySequence<T2>* result = new ArraySequence<T2>();
    for(auto item : *seq){
        result->Append(func(item));
    }
    return result;
}

template <typename T, typename T2>
T2 Reduce(ArraySequence<T>* seq, T2 (*func)(T2, T), T2 start_val){
    T2 result = start_val;
    for(auto item : *seq){
        result = func(result, item);
    }
    return result;
}

template <typename T>
ArraySequence<T>* Where(ArraySequence<T>* seq, bool (*check_func)(T)){
    ArraySequence<T>* result = new ArraySequence<T>();
    for(auto item : *seq){
        if(check_func(item)){
            result->Append(item);
        }
    }
    return result;
}