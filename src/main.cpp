#include <iostream>
#include <functional>

#include "sequence.hpp"
#include "ArraySequence.hpp"
#include "LazySequence.hpp"

int main(){
    ArraySequence<int> seed;
    seed.Append(1);

    std::function<int(Sequence<int>*)> gen = [](Sequence<int>* cache){
        return cache->GetLast() + 1;
    };

    LazySequence<int> naturals(gen, &seed);

    std::cout << naturals.Get(0) << "\n";
    std::cout << naturals.Get(4) << "\n";
    std::cout << naturals.Get(9) << "\n";

    return 0;
}