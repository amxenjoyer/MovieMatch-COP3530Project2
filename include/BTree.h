//
// Created by annap on 11/7/2026.
//

#ifndef MOVIEMATCH_COP3530PROJECT2_BTREE_H
#define MOVIEMATCH_COP3530PROJECT2_BTREE_H

#include <vector>
#include <cstddef>
class BTree {
private:
    struct Node {
        bool isLeaf;
        std::vector<int> keys;
        std::vector<Node*> children;
        explicit Node(bool leaf) : isLeaf(leaf){}
    };
    Node* root = nullptr;
    int minDegree = 3;
    std::size_t movieCount;
    bool searchNode(const Node* node, int movieId) const;
    void insertNonFull(Node* node, int movieId);
    void splitChild(Node* parent, int childIndex);
    void deleteTree(Node* node);

public:
    explicit BTree(int degree = 3);
    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;
    ~BTree();
    bool insert(int movieId);
    bool search(int movieId) const;
    std::size_t size() const;
    bool empty() const;
};

#endif //MOVIEMATCH_COP3530PROJECT2_BTREE_H