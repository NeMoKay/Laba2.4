#pragma once

#include <gtest/gtest.h>
#include <string>
#include <tuple>

#include "ArraySequence.hpp"
#include "LazySequence.hpp"
#include "Stream.hpp"
#include "AlphabetIndex.hpp"

template <typename T1, typename T2>
testing::AssertionResult CheckSize(const T1& actual, const T2& expected){
    if (actual == expected) return testing::AssertionSuccess();
    return testing::AssertionFailure() << "Ожидалось: " << expected << ", По факту: " << actual;
}

template <typename T1, typename T2>
testing::AssertionResult CheckVal(const T1& actual, const T2& expected){
    if (actual == expected) return testing::AssertionSuccess();
    return testing::AssertionFailure() << "Ожидалось: " << expected << ", По факту: " << actual;
}

template <typename T1, typename T2>
testing::AssertionResult CheckBool(const T1& actual, const T2& expected){
    if (actual == expected) return testing::AssertionSuccess();
    return testing::AssertionFailure() << "Ожидалось: " << (expected ? "true" : "false") << ", По факту: " << (actual ? "true" : "false");
}


inline int NaturalGen(Sequence<int>* cache){
    if (cache->GetLength() == 0) return 1;
    return cache->GetLast() + 1;
}

inline int SquareMap(int x){ 
    return x * x; 
}
inline bool IsEven(int x){ 
    return x % 2 == 0; 
}
inline int SumReduce(int acc, int x){
     return acc + x; 
}



class LazySequence_Fixture : public testing::Test{
protected:
    ArraySequence<int>* base_seq;
    LazySequence<int>* finite_lazy;
    LazySequence<int>* infinite_lazy;
    LazySequence<int>* empty_lazy;
    int raw_arr[3] ={1, 2, 3};
    LazySequence<int>* arr_lazy;

    void SetUp() override{
        base_seq = new ArraySequence<int>();
        base_seq->Append(10)->Append(20)->Append(30);
        finite_lazy = new LazySequence<int>(base_seq);
        
        ArraySequence<int>* init_inf = new ArraySequence<int>();
        init_inf->Append(1);
        infinite_lazy = new LazySequence<int>(NaturalGen, init_inf);

        empty_lazy = new LazySequence<int>();
        arr_lazy = new LazySequence<int>(raw_arr, 3);
    }

    void TearDown() override{
        delete base_seq;
        delete finite_lazy;
        delete infinite_lazy;
        delete empty_lazy;
        delete arr_lazy;
    }
};

class Stream_Fixture : public testing::Test{
protected:
    ArraySequence<std::string>* text_seq;
    ReadOnlyStream<std::string>* text_stream;
    LazySequence<int>* lazy_target;
    WriteOnlyStream<int>* write_stream;
    LazySequence<int>* infinite_lazy;
    ReadOnlyStream<int>* lazy_read_stream;

    void SetUp() override{
        text_seq = new ArraySequence<std::string>();
        text_seq->Append("test")->Append("stream")->Append("test")->Append("read");
        text_stream = new ReadOnlyStream<std::string>(text_seq);
        
        lazy_target = new LazySequence<int>();
        write_stream = new WriteOnlyStream<int>(*lazy_target);

        ArraySequence<int>* init_inf = new ArraySequence<int>();
        init_inf->Append(1);
        infinite_lazy = new LazySequence<int>(NaturalGen, init_inf);
        lazy_read_stream = new ReadOnlyStream<int>(infinite_lazy);
    }

    void TearDown() override{
        delete text_seq;
        delete text_stream;
        delete lazy_target;
        delete write_stream;
        delete infinite_lazy;
        delete lazy_read_stream;
    }
};

class AlphavitIndex_Fixture : public testing::Test{
protected:
    ArraySequence<std::string>* text_seq;
    ReadOnlyStream<std::string>* text_stream;
    AlphavitIndex<ArraySequence>* alpha_index;

    void SetUp() override{
        text_seq = new ArraySequence<std::string>();
        text_seq->Append("apple")->Append("banana")->Append("apple")->Append("cherry");
        text_stream = new ReadOnlyStream<std::string>(text_seq);
        alpha_index = new AlphavitIndex<ArraySequence>();
    }

    void TearDown() override{
        delete text_seq;
        delete text_stream;
        delete alpha_index;
    }
};