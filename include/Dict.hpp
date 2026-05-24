#pragma once
#include <cstddef>

template <typename T1, typename T2>
struct Pair{
    T1 elem1;
    T2 elem2;
    Pair(const T1 &new_elem1, const T2 &new_elem2) : elem1(new_elem1), elem2(new_elem2) {}
    Pair() : elem1(T1()), elem2(T2()) {};
    bool operator==(const Pair& other) const{
        return (elem1 == other.elem1) && (elem2 == other.elem2);
    }
};

template <typename K, typename V>
struct bucket{
    Pair<K, V> data;
    bool is_empty;
    int next_idx;
    bucket() : is_empty(true), next_idx(-1) {}
    bucket(Pair<K, V> val) : data(val), is_empty(false), next_idx(-1) {}
};

template <typename K, typename V, template <typename> class Container>
class Dict{
private:
    const int size_table = 100;
    Container<bucket<K, V>> table;

    size_t HashFunc(const K& key) const{
        return (key.elem1 * 31 + key.elem2) % size_table;
    }

public:
    Dict(){
        for (int i = 0; i < size_table; i++){
            table.Append(bucket<K, V>());
        }
    }

    V Get(const K& key) const{
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

    void Set(const K& key, const V& value){
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

    Container<Pair<K, V>> Get_Items() const{
        Container<Pair<K, V>> items;
        for (size_t i = 0; i < table.GetLength(); ++i){
            bucket<K, V> now_elem = table.Get(i);
            if (!now_elem.is_empty){
                items.Append(now_elem.data);
            }
        }
        return items;
    }
};