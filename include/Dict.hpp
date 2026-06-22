#pragma once
#include <cstddef>
#include <functional>

template <typename T1, typename T2>
struct Pair{
    T1 elem1;
    T2 elem2;
    Pair(const T1 &new_elem1, const T2 &new_elem2) : elem1(new_elem1), elem2(new_elem2)  {}
    Pair() : elem1(T1()), elem2(T2())  {};
    bool operator==(const Pair<T1, T2>& other) const{
        return (elem1 == other.elem1) && (elem2 == other.elem2);
    }
};

template <typename K, typename V>
struct bucket{
    Pair<K, V> data;
    bool is_empty;
    int next_idx;
    bucket() : is_empty(true), next_idx(-1)  {}
    bucket(Pair<K, V> val) : data(val), is_empty(false), next_idx(-1)  {}
};

template <typename K, typename V, template <typename> class Container>
class Dict{
private:
    const int size_table = 100;
    Container<bucket<K, V>> table;

    size_t HashFunc(const K& key) const;

public:
    Dict();
    V Get(const K& key) const;
    void Set(const K& key, const V& value);
    Container<Pair<K, V>> Get_Items() const;

    class Iterator{
    private:
        const Dict<K, V, Container>* dict;
        size_t index;
        void SkipZeroVal();

    public:
        Iterator(const Dict<K, V, Container>* dict, size_t index);
        Pair<K, V> operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;
    Iterator end() const;
};

template <typename K, typename V, template <typename> class Container>
size_t Dict<K, V, Container>::HashFunc(const K& key) const{
    std::hash<K> hasher;
    return hasher(key) % size_table;
}

template <typename K, typename V, template <typename> class Container>
Dict<K, V, Container>::Dict(){
    for (int i = 0; i < size_table; i++){
        table.Append(bucket<K, V>());
    }
}

template <typename K, typename V, template <typename> class Container>
V Dict<K, V, Container>::Get(const K& key) const{
    size_t hash = HashFunc(key);
    int index = hash;
    while (index != -1){
        bucket<K, V> now_elem = table.Get(index);
        if (!now_elem.is_empty && now_elem.data.elem1 == key){
            return now_elem.data.elem2;
        }
        index = now_elem.next_idx;
    }
    return V();
}

template <typename K, typename V, template <typename> class Container>
void Dict<K, V, Container>::Set(const K& key, const V& value){
    size_t hash = HashFunc(key);
    bucket<K, V> now_elem = table.Get(hash);

    if (now_elem.is_empty){
        now_elem.data = Pair<K, V>(key, value);
        now_elem.is_empty = false;
        table.Set(hash, now_elem);
        return;
    }

    int index = hash;
    int last_idx = hash;
    while (index != -1){
        now_elem = table.Get(index);
        if (!now_elem.is_empty && now_elem.data.elem1 == key){
            now_elem.data.elem2 = value;
            table.Set(index, now_elem);
            return;
        }
        last_idx = index;
        index = now_elem.next_idx;
    }

    table.Append(bucket<K, V>(Pair<K, V>(key, value)));
    bucket<K, V> last_bucket = table.Get(last_idx);
    last_bucket.next_idx = table.GetLength() - 1;
    table.Set(last_idx, last_bucket);
}

template <typename K, typename V, template <typename> class Container>
Container<Pair<K, V>> Dict<K, V, Container>::Get_Items() const{
    Container<Pair<K, V>> items;
    for (auto now_elem : table){
        if (!now_elem.is_empty){
            items.Append(now_elem.data);
        }
    }
    return items;
}

template <typename K, typename V, template <typename> class Container>
void Dict<K, V, Container>::Iterator::SkipZeroVal(){
    while (index < dict->table.GetLength()){
        if (!dict->table.Get(index).is_empty){
            break;
        }
        index++;
    }
}

template <typename K, typename V, template <typename> class Container>
Dict<K, V, Container>::Iterator::Iterator(const Dict<K, V, Container>* dict, size_t index) : dict(dict), index(index){
    SkipZeroVal();
}

template <typename K, typename V, template <typename> class Container>
Pair<K, V> Dict<K, V, Container>::Iterator::operator*() const{ 
    return dict->table.Get(index).data; 
}

template <typename K, typename V, template <typename> class Container>
typename Dict<K, V, Container>::Iterator& Dict<K, V, Container>::Iterator::operator++(){ 
    index++; 
    SkipZeroVal();
    return *this; 
}

template <typename K, typename V, template <typename> class Container>
bool Dict<K, V, Container>::Iterator::operator!=(const Iterator& other) const{ 
    return index != other.index; 
}

template <typename K, typename V, template <typename> class Container>
typename Dict<K, V, Container>::Iterator Dict<K, V, Container>::begin() const{ 
    return Iterator(this, 0); 
}

template <typename K, typename V, template <typename> class Container>
typename Dict<K, V, Container>::Iterator Dict<K, V, Container>::end() const{ 
    return Iterator(this, table.GetLength()); 
}