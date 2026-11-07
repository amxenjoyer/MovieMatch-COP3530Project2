#include "BTree.h"
#include <algorithm>
#include <stdexcept>
BTree::BTree(int degree) : root(nullptr), minDegree(degree), movieCount(0) {
    if (degree < 2) {
        throw std::invalid_argument("B-Tree minimum degree must be at least 2");
    }
}

BTree::~BTree() {
    deleteTree(root);
}

void BTree::deleteTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    for (Node* child : node->children) {
        deleteTree(child);
    }
    delete node;
}

bool BTree::searchNode(const Node* node, int movieId) const {
    if (node == nullptr) {
        return false;
    }
    auto it = std::lower_bound(node->keys.begin(), node->keys.end(), movieId);
    if (it!=node->keys.end() && *it == movieId) {
        return true;
    }
    if (node->isLeaf) {
        return false;
    }
    int index = static_cast<int>(it - node->keys.begin());
    return searchNode(node->children[index], movieId);
}

bool BTree::search(int movieId) const {
    return searchNode(root, movieId);
}

void BTree::splitChild(Node* parent, int childIndex) {
    Node* fullChild = parent->children[childIndex];
    Node* newChild = new Node(fullChild->isLeaf);
    int middleIndex = minDegree - 1;
    int middleKey = fullChild->keys[middleIndex];
    newChild->keys.assign(fullChild->keys.begin() + minDegree, fullChild->keys.end());
    fullChild->keys.resize(middleIndex);
    if (!fullChild->isLeaf) {
        newChild->children.assign(fullChild->children.begin() + minDegree, fullChild->children.end());
        fullChild->children.resize(minDegree);
    }
    parent->children.insert(parent->children.begin() + childIndex + 1, newChild);
    parent->keys.insert(parent->keys.begin() + childIndex, middleKey);
}

void BTree::insertNonFull(Node* node, int movieId) {
    auto it = std::lower_bound(node->keys.begin(), node->keys.end(), movieId);
    int index = static_cast<int>(it - node->keys.begin());
    if (node->isLeaf) {
        node->keys.insert(it, movieId);
        return;
    }
    if (node->children[index]->keys.size() == static_cast<std::size_t>(2*minDegree-1)) {
        splitChild(node, index);
        if (movieId > node->keys[index]) {
            index++;
        }
    }
    insertNonFull(node->children[index], movieId);
}

bool BTree::insert(int movieId) {
    if (search(movieId)) {
        return false;
    }
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(movieId);
        movieCount++;
        return true;
    }
    if (root->keys.size() == static_cast<std::size_t>(2*minDegree-1)) {
        Node* newRoot = new Node(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0);
        root = newRoot;
    }
    insertNonFull(root, movieId);
    movieCount++;
    return true;
}

std::size_t BTree::size() const {
    return movieCount;
}

bool BTree::empty() const {
    return movieCount == 0;
}

