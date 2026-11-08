#include "BTree.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
    BTree tree;
    assert(tree.empty());
    assert(tree.size() == 0);
    assert(!tree.search(10));
    int movieIds[] = {50, 20, 70, 10, 30, 60, 80, 40, 90, 100};
    for (int id : movieIds) {
        assert(tree.insert(id));
    }
    assert(tree.size() == 10);
    assert(!tree.empty());
    for (int id : movieIds) {
        assert(tree.search(id));
    }
    assert(!tree.search(15));
    assert(!tree.search(55));
    assert(!tree.search(200));
    assert(!tree.insert(50));
    assert(!tree.insert(20));
    assert(tree.size() == 10);

    BTree largeTree;
    for (int id = 1; id <= 1000; id++) {
        assert(largeTree.insert(id));
    }
    assert(largeTree.size() == 1000);

    for (int id = 1; id <= 1000; id++) {
        assert(largeTree.search(id));
    }
    assert(!largeTree.search(1001));

    BTree smallTree(2);
    for (int id = 100; id >= 1; id--) {
        assert(smallTree.insert(id));
    }
    assert(smallTree.size() == 100);

    for (int id = 1; id <= 100; id++) {
        assert(smallTree.search(id));
    }
    bool caughtException = false;

    try {
        BTree invalidTree(1);
    }
    catch (const std::invalid_argument&) {
        caughtException = true;
    }

    assert(caughtException);
    return 0;
}