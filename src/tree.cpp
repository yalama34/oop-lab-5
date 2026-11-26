#include <algorithm>
#include <utility>

#include "tree.hpp"
#include "point.hpp"
#include "selection.hpp"
#include "criterion.hpp"

Tree::Tree(Tree&& other) noexcept
    : tree_(std::move(other.tree_)),
      size_(other.size_) {
    other.size_ = 0;
}

Tree& Tree::operator=(Tree&& other) noexcept {
    if (this != &other) {
        tree_ = std::move(other.tree_);
        size_ = other.size_;
        other.size_ = 0;
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
        return insertNode(node->right, point);
    } else {
        return insertNode(node->left, point);
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

    if (!node->right) {
        auto child = std::move(node->left);
        node = std::move(child);
        return true;
    }

    if (!node->left) {
        auto child = std::move(node->right);
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
    size_--;

    return removeNode(tree_, point);
}

std::size_t Tree::size() const noexcept {return size_;}

std::size_t Tree::heightNode(const std::unique_ptr<Node>& node) const noexcept {
    if (!node) {
        return 0;
    } 
    return 1 + std::max(heightNode(node->left), heightNode(node->right));
}

std::size_t Tree::height() const noexcept {return heightNode(tree_);}

void Tree::inorderNode(
    const std::unique_ptr<Node>& node, 
    Selection& selection,
    const Criterion* criterion) const {
    if (!node) {
        return;
    }
    inorderNode(node->left, selection, criterion);
    if (!criterion || criterion->matches(*node->data)) {
        selection.add(node->data);
    } 
    inorderNode(node->right, selection, criterion);
}

Selection Tree::inorder() const{
    Selection selection;
    
    inorderNode(tree_, selection, nullptr);

    return selection;
}

void Tree::reverseInorderNode(
    const std::unique_ptr<Node>& node, 
    Selection& selection, 
    const Criterion* criterion) const {
    if (!node) {
        return;
    }
    reverseInorderNode(node->right, selection, criterion);
    if (!criterion || criterion->matches(*node->data)) {
        selection.add(node->data);
    } 
    reverseInorderNode(node->left, selection, criterion);
}

Selection Tree::reverseInorder() const {
    Selection selection;

    reverseInorderNode(tree_, selection, nullptr);

    return selection;
}
 
Selection Tree::select(const Criterion& criterion) const {
    Selection selection;

    inorderNode(tree_, selection, &criterion);

    return selection;
} 