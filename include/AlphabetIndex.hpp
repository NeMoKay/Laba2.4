#pragma once

#include <string>
#include "Stream.hpp"
#include "Dict.hpp"
#include "ArraySequence.hpp"

template <template <typename> class Container>
class AlphavitIndex{
private:
    Dict<std::string, ArraySequence<size_t>, Container> indexDict;

public:
    AlphavitIndex();

    void BuildFromStream(ReadOnlyStream<std::string>& stream);
    void AddWord(const std::string& word, size_t position);
    void Clear();
    
    ArraySequence<size_t> GetWordPositions(const std::string& word) const;
    Container<Pair<std::string, ArraySequence<size_t>>> GetAllEntries() const;
};


template <template <typename> class Container>
AlphavitIndex<Container>::AlphavitIndex() {}

template <template <typename> class Container>
void AlphavitIndex<Container>::BuildFromStream(ReadOnlyStream<std::string>& stream){
    stream.Open();
    while(!stream.IsEndOfStream()){
        std::string word = stream.Read();
        size_t currentPosition = stream.GetPosition() - 1; 
        AddWord(word, currentPosition);
    }
    stream.Close();
}

template <template <typename> class Container>
void AlphavitIndex<Container>::AddWord(const std::string& word, size_t position){
    ArraySequence<size_t> positions = indexDict.Get(word);
    positions.Append(position);
    indexDict.Set(word, positions);
}

template <template <typename> class Container>
void AlphavitIndex<Container>::Clear(){
    indexDict = Dict<std::string, ArraySequence<size_t>, Container>();
}

template <template <typename> class Container>
ArraySequence<size_t> AlphavitIndex<Container>::GetWordPositions(const std::string& word) const{
    return indexDict.Get(word);
}

template <template <typename> class Container>
Container<Pair<std::string, ArraySequence<size_t>>> AlphavitIndex<Container>::GetAllEntries() const{
    return indexDict.Get_Items();
}