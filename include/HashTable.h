#ifndef MOVIEMATCH_HASHTABLE_H
#define MOVIEMATCH_HASHTABLE_H

#include <vector>
#include <cstddef>

#include "RatingKey.h"

class HashTable {
private:
    enum class SlotState {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    struct Entry {
        RatingKey key;
        size_t ratingIndex;
        SlotState state = SlotState::EMPTY;
    };

    std::vector<Entry> table;
    size_t capacity;
    size_t size;

    // calculate initial index for key
    size_t hashFunction(const RatingKey& key) const;

    // resize table and reinsert existing entries
    void rehash();

public:
    //constructor
    explicit HashTable(size_t initialCapacity = 16);

    // insert key and its rating index
    bool insert(const RatingKey& key, size_t ratingIndex);

    // find key and get its rating index
    bool search(const RatingKey& key, size_t& ratingIndex) const;

    // remove entry from table
    bool remove(const RatingKey& key);

    // return num of entries
    size_t getSize() const;

    // return table capacity
    size_t getCapacity() const;
};
#endif //MOVIEMATCH_HASHTABLE_H
