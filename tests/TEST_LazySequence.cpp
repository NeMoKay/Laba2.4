#include "Fixtures.hpp"
#include <tuple>
#include <iostream>

class LazyConstructorTest : public LazySequence_Fixture, 
                            public testing::WithParamInterface<std::tuple<size_t, int>> {};

TEST_P(LazyConstructorTest, arr_constructor){
    auto [index, expected] = GetParam();
    EXPECT_TRUE(CheckVal(arr_lazy->Get(index), expected));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование конструктора LazySequence от массива---\n";
        std::cout << "Исходный массив:{1, 2, 3}\n";
        std::cout << "Запрашиваемый индекс: " << index << "\n";
        std::cout << "Ожидаемое значение: " << expected << "\n";
    }
}
INSTANTIATE_TEST_SUITE_P(LazyArrParams, LazyConstructorTest, testing::Values(
    std::make_tuple(0, 1),
    std::make_tuple(1, 2),
    std::make_tuple(2, 3)
));

TEST_F(LazySequence_Fixture, copy_constructor){
    LazySequence<int> copy_lazy(*finite_lazy);
    EXPECT_TRUE(CheckVal(copy_lazy.Get(0), 10));
    EXPECT_TRUE(CheckVal(copy_lazy.Get(2), 30));
    EXPECT_TRUE(CheckSize(copy_lazy.GetLength().GetValue(), 3));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование конструктора копирования LazySequence---\n";
        std::cout << "Исходная последовательность:{10, 20, 30}\n";
    }
}

class LazyElementsTest : public LazySequence_Fixture, 
                         public testing::WithParamInterface<std::tuple<size_t, int>> {};

TEST_P(LazyElementsTest, get_and_operator){
    auto [index, expected] = GetParam();
    EXPECT_TRUE(CheckVal(finite_lazy->Get(index), expected));
    EXPECT_TRUE(CheckVal((*finite_lazy)[index], expected));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование доступа к элементам (Get и оператор [])---\n";
        std::cout << "Конечная последовательность:{10, 20, 30}\n";
        std::cout << "Запрашиваемый индекс: " << index << "\n";
        std::cout << "Ожидаемое значение: " << expected << "\n";
    }
}
INSTANTIATE_TEST_SUITE_P(LazyFiniteParams, LazyElementsTest, testing::Values(
    std::make_tuple(0, 10),
    std::make_tuple(1, 20),
    std::make_tuple(2, 30)
));

TEST_F(LazySequence_Fixture, get_first_last){
    EXPECT_TRUE(CheckVal(finite_lazy->GetFirst(), 10));
    EXPECT_TRUE(CheckVal(finite_lazy->GetLast(), 30));
    EXPECT_THROW(empty_lazy->GetFirst(), EmptySequenceException);
    EXPECT_THROW(empty_lazy->GetLast(), EmptySequenceException);
    EXPECT_THROW(infinite_lazy->GetLast(), Exception);

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование методов GetFirst() и GetLast()---\n";
        std::cout << "Конечная последовательность:{10, 20, 30}\n";
        std::cout << "Ожидался корректный вызов или исключение для граничных случаев (пустой список, бесконечный список).\n";
    }
}

class LazyInfiniteTest : public LazySequence_Fixture, 
                         public testing::WithParamInterface<std::tuple<size_t, int>> {};

TEST_P(LazyInfiniteTest, materialization){
    auto [index, expected] = GetParam();
    EXPECT_TRUE(CheckVal(infinite_lazy->Get(index), expected));
    EXPECT_TRUE(CheckSize(infinite_lazy->GetMaterializedCount(), index + 1));
    EXPECT_TRUE(CheckBool(infinite_lazy->GetLength().IsInfinite(), true));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование материализации элементов бесконечной LazySequence---\n";
        std::cout << "Используется генератор натуральных чисел:{1, 2, 3...}\n";
        std::cout << "Запрашиваемый индекс: " << index << "\n";
        std::cout << "Ожидаемое значение: " << expected << "\n";
    }
}
INSTANTIATE_TEST_SUITE_P(LazyInfiniteParams, LazyInfiniteTest, testing::Values(
    std::make_tuple(0, 1),
    std::make_tuple(4, 5),
    std::make_tuple(99, 100)
));

class LazyModifiersTest : public LazySequence_Fixture, 
                          public testing::WithParamInterface<std::tuple<int, size_t, int>> {};

TEST_P(LazyModifiersTest, append_prepend_insert){
    auto [val, insert_idx, check_val] = GetParam();
    
    empty_lazy->Append(val);
    EXPECT_TRUE(CheckVal(empty_lazy->GetLast(), val));
    
    finite_lazy->Prepend(val);
    EXPECT_TRUE(CheckVal(finite_lazy->GetFirst(), val));
    
    arr_lazy->InsertAt(val, insert_idx);
    EXPECT_TRUE(CheckVal(arr_lazy->Get(insert_idx), val));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование модификаторов (Append, Prepend, InsertAt)---\n";
        std::cout << "Вставляемое значение: " << val << "\n";
        std::cout << "Индекс для InsertAt в arr_lazy: " << insert_idx << "\n";
    }
}
INSTANTIATE_TEST_SUITE_P(LazyModParams, LazyModifiersTest, testing::Values(
    std::make_tuple(99, 1, 99),
    std::make_tuple(42, 0, 42)
));

TEST_F(LazySequence_Fixture, concat_and_subsequence){
    finite_lazy->Concat(arr_lazy);
    EXPECT_TRUE(CheckSize(finite_lazy->GetLength().GetValue(), 6));
    EXPECT_TRUE(CheckVal(finite_lazy->Get(3), 1));
    EXPECT_TRUE(CheckVal(finite_lazy->Get(5), 3));
    
    LazySequence<int>* sub = finite_lazy->GetSubsequence(1, 4);
    EXPECT_TRUE(CheckSize(sub->GetLength().GetValue(), 4));
    EXPECT_TRUE(CheckVal(sub->Get(0), 20));
    EXPECT_TRUE(CheckVal(sub->GetLast(), 2));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование методов Concat и GetSubsequence---\n";
        std::cout << "Конкатенация:{10, 20, 30} и{1, 2, 3}\n";
        std::cout << "Ожидаемый размер после Concat: 6\n";
        std::cout << "GetSubsequence от индекса 1 до 4. Ожидаемый результат:{20, 30, 1, 2}\n";
    }
    
    delete sub;
}

TEST_F(LazySequence_Fixture, modifiers_exceptions){
    EXPECT_THROW(infinite_lazy->Append(10), Exception);
    EXPECT_THROW(infinite_lazy->Concat(finite_lazy), Exception);
    EXPECT_THROW(finite_lazy->InsertAt(10, 100), IndexOutOfRangeException);
    EXPECT_THROW(finite_lazy->GetSubsequence(2, 1), IndexOutOfRangeException);

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование исключений при модификации---\n";
        std::cout << "Ожидалась генерация исключений при операциях с бесконечным списком или неверными индексами.\n";
    }
}

class LazyFunctionalTest : public LazySequence_Fixture, 
                           public testing::WithParamInterface<std::tuple<size_t, int>> {};

TEST_P(LazyFunctionalTest, map_func){
    auto [index, expected] = GetParam();
    LazySequence<int>* mapped = finite_lazy->Map(SquareMap, finite_lazy->GetLength().GetValue());
    EXPECT_TRUE(CheckVal(mapped->Get(index), expected));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование функции Map (возведение в квадрат)---\n";
        std::cout << "Исходная последовательность:{10, 20, 30}\n";
        std::cout << "Проверяемый индекс: " << index << "\n";
        std::cout << "Ожидаемое значение: " << expected << "\n";
    }
    delete mapped;
}
INSTANTIATE_TEST_SUITE_P(LazyMapParams, LazyFunctionalTest, testing::Values(
    std::make_tuple(0, 100),
    std::make_tuple(1, 400),
    std::make_tuple(2, 900)
));

TEST_F(LazySequence_Fixture, where_and_reduce){
    LazySequence<int>* filtered = finite_lazy->Where(IsEven, finite_lazy->GetLength().GetValue());
    EXPECT_TRUE(CheckSize(filtered->GetLength().GetValue(), 3));
    EXPECT_TRUE(CheckVal(filtered->Get(1), 20));
    
    int sum = finite_lazy->Reduce(SumReduce, 0, finite_lazy->GetLength().GetValue());
    EXPECT_TRUE(CheckVal(sum, 60));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование функций Where (IsEven) и Reduce (Sum)---\n";
        std::cout << "Исходная последовательность:{10, 20, 30}\n";
        std::cout << "Ожидаемая сумма: 60\n";
    }
    
    delete filtered;
}