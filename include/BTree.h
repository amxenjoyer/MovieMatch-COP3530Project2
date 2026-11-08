
#ifndef MOVIEMATCH_COP3530PROJECT2_BTREE_H
#define MOVIEMATCH_COP3530PROJECT2_BTREE_H

#include <vector>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include "RatingKey.h"

template <typename Key>
class BTreeBase {
private:
    struct Node {
        bool isLeaf;
        std::vector<Key> keys;
        std::vector<Node*> children;

        explicit Node(bool leaf) : isLeaf(leaf) {}
    };

    Node* root = nullptr;
    int minDegree = 3;
    std::size_t keyCount = 0;

    void deleteTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        for (Node* child : node->children) {
            deleteTree(child);
        }

        delete node;
    }

    bool searchNode(const Node* node, const Key& key) const {
        if (node == nullptr) {
            return false;
        }

        auto it = std::lower_bound(
            node->keys.begin(),
            node->keys.end(),
            key
        );

        if (it != node->keys.end() &&
            !(key < *it) && !(*it < key)) {
            return true;
        }

        if (node->isLeaf) {
            return false;
        }

        int index = static_cast<int>(
            it - node->keys.begin()
        );

        return searchNode(node->children[index], key);
    }

    void splitChild(Node* parent, int childIndex) {
        Node* fullChild = parent->children[childIndex];
        Node* newChild = new Node(fullChild->isLeaf);

        int middleIndex = minDegree - 1;
        Key middleKey = fullChild->keys[middleIndex];

        newChild->keys.assign(
            fullChild->keys.begin() + minDegree,
            fullChild->keys.end()
        );

        fullChild->keys.resize(middleIndex);

        if (!fullChild->isLeaf) {
            newChild->children.assign(
                fullChild->children.begin() + minDegree,
                fullChild->children.end()
            );

            fullChild->children.resize(minDegree);
        }

        parent->children.insert(
            parent->children.begin() + childIndex + 1,
            newChild
        );

        parent->keys.insert(
            parent->keys.begin() + childIndex,
            middleKey
        );
    }

    void insertNonFull(Node* node, const Key& key) {
        auto it = std::lower_bound(
            node->keys.begin(),
            node->keys.end(),
            key
        );

        int index = static_cast<int>(
            it - node->keys.begin()
        );

        if (node->isLeaf) {
            node->keys.insert(it, key);
            return;
        }

        if (node->children[index]->keys.size() ==
            static_cast<std::size_t>(2 * minDegree - 1)) {

            splitChild(node, index);

            if (node->keys[index] < key) {
                index++;
            }
        }

        insertNonFull(node->children[index], key);
    }

public:
    explicit BTreeBase(int degree = 3)
        : minDegree(degree) {

        if (degree < 2) {
            throw std::invalid_argument(
                "B-Tree minimum degree must be at least 2"
            );
        }
    }

    BTreeBase(const BTreeBase&) = delete;
    BTreeBase& operator=(const BTreeBase&) = delete;

    ~BTreeBase() {
        deleteTree(root);
    }

    bool search(const Key& key) const {
        return searchNode(root, key);
    }

    bool insert(const Key& key) {
        if (search(key)) {
            return false;
        }

        if (root == nullptr) {
            root = new Node(true);
            root->keys.push_back(key);
            keyCount++;
            return true;
        }

        if (root->keys.size() ==
            static_cast<std::size_t>(2 * minDegree - 1)) {

            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);

            splitChild(newRoot, 0);
            root = newRoot;
        }

        insertNonFull(root, key);
        keyCount++;

        return true;
    }

    std::size_t size() const {
        return keyCount;
    }

    bool empty() const {
        return keyCount == 0;
    }
};

// Original movie-ID B-Tree
using BTree = BTreeBase<int>;

// New B-Tree for user/movie rating keys
using RatingBTree = BTreeBase<RatingKey>;

#endif
