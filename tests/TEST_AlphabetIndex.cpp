#include "Fixtures.hpp"
#include <tuple>

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
}