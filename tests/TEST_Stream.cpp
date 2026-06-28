#include "Fixtures.hpp"
#include <tuple>

class StreamReadTest : public Stream_Fixture, 
                       public testing::WithParamInterface<std::tuple<size_t, std::string>> {};

TEST_P(StreamReadTest, read_sequential){
    auto [iterations, expected_val] = GetParam();
    
    text_stream->Open();
    std::string val;
    
    for(size_t i = 0; i <= iterations; i++){
        val = text_stream->Read();
    }
    
    EXPECT_TRUE(CheckVal(val, expected_val));
    EXPECT_TRUE(CheckSize(text_stream->GetPosition(), iterations + 1));
    EXPECT_TRUE(CheckBool(text_stream->IsCanSeek(), true));
    EXPECT_TRUE(CheckBool(text_stream->IsCanGoBack(), true));
    
    text_stream->Close();
    EXPECT_TRUE(CheckSize(text_stream->GetPosition(), 0));
}
INSTANTIATE_TEST_SUITE_P(StreamReadsParams, StreamReadTest, testing::Values(
    std::make_tuple(0, "test"),
    std::make_tuple(1, "stream"),
    std::make_tuple(3, "read")
));

TEST_F(Stream_Fixture, stream_seek){
    text_stream->Open();
    text_stream->Seek(2);
    EXPECT_TRUE(CheckSize(text_stream->GetPosition(), 2));
    EXPECT_TRUE(CheckVal(text_stream->Read(), "test"));
    EXPECT_THROW(text_stream->Seek(10), IndexOutOfRangeException);
}

TEST_F(Stream_Fixture, read_only_exceptions){
    text_stream->Open();
    for(int i = 0; i < 4; i++){
        text_stream->Read();
    }
    EXPECT_TRUE(CheckBool(text_stream->IsEndOfStream(), true));
    EXPECT_THROW(text_stream->Read(), Exception);
}

class StreamLazyTest : public Stream_Fixture, 
                       public testing::WithParamInterface<std::tuple<size_t, int>> {};

TEST_P(StreamLazyTest, read_from_infinite_lazy){
    auto [iterations, expected_val] = GetParam();
    
    lazy_read_stream->Open();
    int val = 0;
    
    for(size_t i = 0; i <= iterations; i++){
        val = lazy_read_stream->Read();
    }
    
    EXPECT_TRUE(CheckVal(val, expected_val));
    EXPECT_TRUE(CheckBool(lazy_read_stream->IsEndOfStream(), false));
    lazy_read_stream->Close();
}
INSTANTIATE_TEST_SUITE_P(StreamLazyParams, StreamLazyTest, testing::Values(
    std::make_tuple(0, 1),
    std::make_tuple(4, 5),
    std::make_tuple(49, 50)
));

class StreamWriteTest : public Stream_Fixture, 
                        public testing::WithParamInterface<std::tuple<int, size_t>> {};

TEST_P(StreamWriteTest, write_to_lazy){
    auto [val, expected_pos] = GetParam();
    
    write_stream->Open();
    size_t pos = write_stream->Write(val);
    
    EXPECT_TRUE(CheckSize(pos, expected_pos));
    EXPECT_TRUE(CheckSize(write_stream->GetPosition(), expected_pos));
    EXPECT_TRUE(CheckVal(lazy_target->Get(expected_pos - 1), val));
    
    write_stream->Close();
    EXPECT_TRUE(CheckSize(write_stream->GetPosition(), 0));
}
INSTANTIATE_TEST_SUITE_P(StreamWriteParams, StreamWriteTest, testing::Values(
    std::make_tuple(10, 1),
    std::make_tuple(20, 1),
    std::make_tuple(30, 1)
));