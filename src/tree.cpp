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

std::weak_ptr<Point> Tree::findNode(const std::unique_ptr<Node>& node, const Point& point) const {
    if (!node) {
        return {};
    }
    if (point < *node->data) {
        return findNode(node->left, point);
    }
    if (*node->data < point) {
        return findNode(node->right, point);
    }
    
    return node->data;
}

std::weak_ptr<Point> Tree::find(const Point& point) const {
    return findNode(tree_, point);
}

bool Tree::removeNode(std::unique_ptr<Node>& node, const Point& point) {
    if (!node) {
        return false;
    }

    if (point < *node->data) {
        return removeNode(node->left, point);
    }
    if (*node->data < point) {
        return removeNode(node->right, point);
    } 

    if (node->left == nullptr && node->right == nullptr) {
        node.reset();
        return true;
    }
    if (node->right == nullptr) {
        auto child = std::move(node->right);
        node = std::move(child);
        return true;
    }
    if (node->left == nullptr) {
        auto child = std::move(node->left);
        node = std::move(child);
        return true;
    }
    
    Node* successor = node->right.get();
    while (successor->left) {
        successor = successor->left.get();
    }
    
    node->data = successor->data;

    return removeNode(node->right, *node->data);
}

bool Tree::remove(const Point& point) {
    if (find(point).expired()) {
        return false;
    }
    
    return removeNode(tree_, point);
}