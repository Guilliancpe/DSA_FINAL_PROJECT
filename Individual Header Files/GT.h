#ifndef GT_H
#define GT_H

#include <iostream>
#include <vector>

// General tree using left = first child, right = next sibling
template <typename T>
class genTree {
    private:
        T key;
        genTree<T>* left;
        genTree<T>* right;
        genTree<T>* findData;

        void find(T value);

    public:
        genTree(T value);
        ~genTree();

        bool insertChild(T parentValue, T childValue);
        std::vector<T> getChildren(T parentValue);
        void display(int depth = 0);
};

// ---------------
// Definitions
// ---------------

template <typename T>
genTree<T>::genTree(T value)
    : key(value), left(nullptr), right(nullptr), findData(nullptr) {}

template <typename T>
genTree<T>::~genTree() {
    delete left;
    delete right;
}

template <typename T>
void genTree<T>::find(T value) {
    if (key == value) {
        findData = this;
        return;
    }

    findData = nullptr;

    if (left != nullptr) {
        left->find(value);
        if (left->findData != nullptr) {
            findData = left->findData;
            return;
        }
    }

    if (right != nullptr) {
        right->find(value);
        if (right->findData != nullptr) {
            findData = right->findData;
            return;
        }
    }
}

template <typename T>
bool genTree<T>::insertChild(T parentValue, T childValue) {
    find(parentValue);
    genTree<T>* parent = findData;

    if (parent == nullptr) {
        std::cout << "Parent not found!" << std::endl;
        return false;
    }

    genTree<T>* newNode = new genTree<T>(childValue);

    if (parent->left == nullptr) {
        parent->left = newNode;
    } else {
        genTree<T>* sibling = parent->left;
        while (sibling->right != nullptr) {
            sibling = sibling->right;
        }
        sibling->right = newNode;
    }
    return true;
}

// returns every child of the node holding parentValue (first child, then its siblings)
template <typename T>
std::vector<T> genTree<T>::getChildren(T parentValue) {
    std::vector<T> kids;

    find(parentValue);
    genTree<T>* parent = findData;
    if (parent == nullptr) return kids;

    for (genTree<T>* cur = parent->left; cur != nullptr; cur = cur->right) {
        kids.push_back(cur->key);
    }
    return kids;
}

template <typename T>
void genTree<T>::display(int depth) {
    for (int i = 0; i < depth; i++) std::cout << "   ";
    std::cout << key << std::endl;

    if (left != nullptr) left->display(depth + 1);
    if (right != nullptr) right->display(depth);
}

#endif
