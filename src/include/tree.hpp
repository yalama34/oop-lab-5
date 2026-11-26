#pragma once

#include <memory>
#include "point.hpp"
#include "selection.hpp"
#include "criterion.hpp"

struct Node {
    std::shared_ptr<Point> data;

    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

class Tree {
    std::unique_ptr<Node> tree_;
    std::size_t size_ = 0;
    bool insertNode(std::unique_ptr<Node>& node, const Point& point);
    std::weak_ptr<Point> findNode(const std::unique_ptr<Node>& node, const Point& point) const;
    bool removeNode(std::unique_ptr<Node>& node, const Point& point);

public:
    Tree() = default;
    ~Tree() = default;

    Tree(const Tree& other) = delete;
    Tree& operator=(const Tree& other) = delete;

    Tree(Tree&& other) noexcept;
    Tree& operator=(Tree&& other) noexcept;

    bool insert(const Point& p);
    std::weak_ptr<Point> find(const Point& p) const;
    bool remove(const Point& p);
    
    std::size_t size() const noexcept;
    std::size_t height() const noexcept;

    Selection select(const Criterion& criterion) const;
    Selection inorder() const;
    Selection reverseInorder() const;
};