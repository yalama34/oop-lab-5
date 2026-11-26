#include "tree.hpp"
#include "point.hpp"

Tree::Tree(Tree&& other) noexcept
    : size_(other.size_){
    tree_ = std::move(other.tree_);
}

Tree& Tree::operator=(Tree&& other) noexcept {
    if (this != &other) {
        size_ = other.size();
        tree_ = std::move(other.tree_);
    }
    
    return *this;
}

bool Tree::insertNode(std::unique_ptr<Node>& node, const Point& point) {
    if (!node) {
        auto new_node = std::make_unique<Node>();
        new_node->data = std::make_shared<Point>(point);

        node = std::move(new_node);

        return true;
    }
    if (*node->data < point) {
        return insertNode(node->left, point);
    } else {
        return insertNode(node->right, point);
    }
}

bool Tree::insert(const Point& point) {
    if (insertNode(tree_, point)) {
        size_++;
        return true;
    }

    return false;
}