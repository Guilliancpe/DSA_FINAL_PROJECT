#ifndef BST_H
#define BST_H

#include <iostream>
#include <vector>

template <typename T>
class binarySearchTree {
    private:
        T key;
        binarySearchTree<T>* left;
        binarySearchTree<T>* right;

        void inOrder(std::vector<T>& out);

    public:
        binarySearchTree(T value);
        ~binarySearchTree();

        // smaller goes left, equal or bigger goes right (so nobody gets dropped)
        void insert(T value);

        // everything in the left / right subtree of this node, in sorted order
        void collectLeft(std::vector<T>& out);
        void collectRight(std::vector<T>& out);

        // sideways picture of the tree (right side on top)
        void display(int depth = 0);

        // number of levels from this node down to its deepest leaf
        int height();
};

// ---------------
// Definitions
// ---------------

template <typename T>
binarySearchTree<T>::binarySearchTree(T value)
    : key(value), left(nullptr), right(nullptr) {}

template <typename T>
binarySearchTree<T>::~binarySearchTree() {
    delete left;
    delete right;
}

template <typename T>
void binarySearchTree<T>::insert(T value) {
    if (value < key) {
        if (left == nullptr) left = new binarySearchTree<T>(value);
        else left->insert(value);
    } else {
        if (right == nullptr) right = new binarySearchTree<T>(value);
        else right->insert(value);
    }
}

template <typename T>
void binarySearchTree<T>::inOrder(std::vector<T>& out) {
    if (left != nullptr) left->inOrder(out);
    out.push_back(key);
    if (right != nullptr) right->inOrder(out);
}

template <typename T>
void binarySearchTree<T>::collectLeft(std::vector<T>& out) {
    if (left != nullptr) left->inOrder(out);
}

template <typename T>
void binarySearchTree<T>::collectRight(std::vector<T>& out) {
    if (right != nullptr) right->inOrder(out);
}

template <typename T>
void binarySearchTree<T>::display(int depth) {
    if (right != nullptr) right->display(depth + 1);
    for (int i = 0; i < depth; i++) std::cout << "      ";
    std::cout << key << std::endl;
    if (left != nullptr) left->display(depth + 1);
}

template <typename T>
int binarySearchTree<T>::height() {
    int leftHeight  = (left  != nullptr) ? left->height()  : 0;
    int rightHeight = (right != nullptr) ? right->height() : 0;
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

#endif
