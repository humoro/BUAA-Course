#ifndef COMPILER7_2_IROPTIMAL_H
#define COMPILER7_2_IROPTIMAL_H
#include <iostream>
#include <utility>
using namespace std;
class dagNode{
    string symbol;
    dagNode* left;
    dagNode* right;
    dagNode* parent;
    dagNode(string symbol): symbol(std::move(symbol)), left(nullptr), right(nullptr), parent(nullptr){};

public:
    void setLeft(dagNode* leftNode) {
        this->left = leftNode;
    }

    void setRight(dagNode* rightNode) {
        this->right = rightNode;
    }

    void setParent(dagNode* parentNode) {
        this->parent = parentNode;
    }

    void removeParent() {
        this->parent = nullptr;
    }

    bool isRoot() {
        return this->parent == nullptr;
    }

    dagNode* getLeft() {
        return this->left;
    }

    dagNode* getRight() {
        return this->right;
    }
};
#endif
