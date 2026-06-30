#include "Fixtures.hpp"
#include <tuple>
#include <iostream>

class AlphavitIndexSearchTest : public AlphavitIndex_Fixture, 
                                public testing::WithParamInterface<std::tuple<std::string, size_t, size_t>> {};

TEST_P(AlphavitIndexSearchTest, build_and_search){
    alpha_index->BuildFromStream(*text_stream);
    auto [word, expected_count, first_pos] = GetParam();
    
    ArraySequence<size_t> positions = alpha_index->GetWordPositions(word);
    
    EXPECT_TRUE(CheckSize(positions.GetLength(), expected_count));
    
    if (expected_count > 0){
        EXPECT_TRUE(CheckVal(positions.Get(0), first_pos));
    }

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование построения и поиска в Алфавитном указателе---\n";
        std::cout << "Исходный поток слов: apple banana apple cherry\n";
        std::cout << "Искомое слово: " << word << "\n";
        std::cout << "Ожидаемое количество вхождений: " << expected_count << "\n";
        if (expected_count > 0){
            std::cout << "Ожидаемый индекс первого вхождения: " << first_pos << "\n";
        }
    }
}
INSTANTIATE_TEST_SUITE_P(AlphavitIndexParams, AlphavitIndexSearchTest, testing::Values(
    std::make_tuple("apple", 2, 0),
    std::make_tuple("banana", 1, 1),
    std::make_tuple("cherry", 1, 3),
    std::make_tuple("missing", 0, 0)
));

TEST_F(AlphavitIndex_Fixture, get_all_entries){
    alpha_index->BuildFromStream(*text_stream);
    
    auto all_entries = alpha_index->GetAllEntries();
    
    EXPECT_TRUE(CheckSize(all_entries.GetLength(), 3));

    if (testing::Test::HasFailure()){
        std::cout << "\n---Тестирование получения всех записей---\n";
        std::cout << "Ожидалось количество уникальных слов в словаре: 3 (apple, banana, cherry)\n";
        std::cout << "Фактически получено: " << all_entries.GetLength() << "\n";
    }
}