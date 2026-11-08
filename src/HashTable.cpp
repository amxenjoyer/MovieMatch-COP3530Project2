
#include "HashTable.h"

#include <functional>
#include <cstdint>

HashTable::HashTable(size_t initialCapacity) {
    capacity = (initialCapacity == 0) ? 1 : initialCapacity;
    size = 0;

    table.resize(capacity);
}

size_t HashTable::hashFunction(const RatingKey& key) const {
    // Combine user ID and movie ID into one 64-bit value
    uint64_t combined =
        (static_cast<uint64_t>(
            static_cast<uint32_t>(key.userId)) << 32) |
        static_cast<uint32_t>(key.movieId);

    // Mix bits to distribute keys more evenly, using finalizer from SplitMix64
    combined += 0x9e3779b97f4a7c15ULL;
    combined = (combined ^ (combined >> 30))
               * 0xbf58476d1ce4e5b9ULL;
    combined = (combined ^ (combined >> 27))
               * 0x94d049bb133111ebULL;
    combined ^= (combined >> 31);

    return static_cast<size_t>(combined % capacity);
}

bool HashTable::insert(const RatingKey& key, size_t ratingIndex) {
    size_t index = hashFunction(key);
    size_t firstDeleted = capacity;

    // check for duplicate keys, find available position
    for (size_t i = 0; i < capacity; i++) {
        size_t currentIndex = (index + i) % capacity;

        if (table[currentIndex].state == SlotState::EMPTY) {
            break;
        }

        if (table[currentIndex].state == SlotState::OCCUPIED) {
            if (table[currentIndex].key == key) {
                table[currentIndex].ratingIndex = ratingIndex;
                return true;
            }
        }
        else if (table[currentIndex].state == SlotState::DELETED) {
            if (firstDeleted == capacity) {
                firstDeleted = currentIndex;
            }
        }
    }

    // check load factor before insertion
    if (static_cast<double>(size + 1) / capacity >= 0.7) {
        rehash();

        // recalculate index because capacity changed
        index = hashFunction(key);
        firstDeleted = capacity;
    }

    // find available position
    for (size_t i = 0; i < capacity; i++) {
        size_t currentIndex = (index + i) % capacity;

        if (table[currentIndex].state == SlotState::EMPTY) {
            if (firstDeleted != capacity) {
                currentIndex = firstDeleted;
            }

            table[currentIndex].key = key;
            table[currentIndex].ratingIndex = ratingIndex;
            table[currentIndex].state = SlotState::OCCUPIED;

            size++;
            return true;
        }

        if (table[currentIndex].state == SlotState::DELETED &&
            firstDeleted == capacity) {
            firstDeleted = currentIndex;
        }
    }

    // use deleted position if no empty position found
    if (firstDeleted != capacity) {
        table[firstDeleted].key = key;
        table[firstDeleted].ratingIndex = ratingIndex;
        table[firstDeleted].state = SlotState::OCCUPIED;

        size++;
        return true;
    }

    return false;
}

bool HashTable::search(const RatingKey& key, size_t& ratingIndex) const {
    size_t index = hashFunction(key);

    for (size_t i = 0; i < capacity; i++) {
        size_t currentIndex = (index + i) % capacity;

        // empty slot = key doesn't exist
        if (table[currentIndex].state == SlotState::EMPTY) {
            return false;
        }

        // check if current entry matches
        if (table[currentIndex].state == SlotState::OCCUPIED &&
            table[currentIndex].key == key) {
            ratingIndex = table[currentIndex].ratingIndex;
            return true;
        }
    }

    return false;
}

bool HashTable::remove(const RatingKey& key) {
    size_t index = hashFunction(key);

    for (size_t i = 0; i < capacity; i++) {
        size_t currentIndex = (index + i) % capacity;

        // empty slot = key doesn't exist
        if (table[currentIndex].state == SlotState::EMPTY) {
            return false;
        }

        // find matching key
        if (table[currentIndex].state == SlotState::OCCUPIED &&
            table[currentIndex].key == key) {
            table[currentIndex].state = SlotState::DELETED;
            size--;

            return true;
        }
    }

    return false;
}

void HashTable::rehash() {
    //save current table
    std::vector<Entry> oldTable = table;

    //double capacity
    capacity *= 2;

    //create new empty table
    table.clear();
    table.resize(capacity);

    //reset num of occupied entries
    size = 0;

    // reinsert occupied entries
    for (const Entry& entry : oldTable) {
        if (entry.state == SlotState::OCCUPIED) {
            size_t index = hashFunction(entry.key);

            // find next available position
            while (table[index].state == SlotState::OCCUPIED) {
                index = (index + 1) % capacity;
            }

            table[index].key = entry.key;
            table[index].ratingIndex = entry.ratingIndex;
            table[index].state = SlotState::OCCUPIED;

            size++;
        }
    }
}

size_t HashTable::getSize() const {
    return size;
}

size_t HashTable::getCapacity() const {
    return capacity;
}
