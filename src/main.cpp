#include <iostream>

#include "sequence.hpp"
#include "ArraySequence.hpp"
#include "LinkedList.hpp"
int main(){
    int A[5] ={1,2,3,4,5};
    ArraySequence<int> B(A);

    std::cout << B.GetLength();



    return 0;
}






