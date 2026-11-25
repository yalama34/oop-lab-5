#pragma once

#include <memory>
#include "point.hpp"

class Selection {
    std::unique_ptr<std::weak_ptr<Point>[]> data_;
    std::size_t size;
    std::size_t capacity;

    void reserve(std::size_t new_capacity);

public:
    Selection() = default;
    ~Selection() = default;
    
    Selection(const Selection& other);
    Selection& operator=(const Selection& other);
    
    Selection(Selection&& other) noexcept;
    Selection& operator=(Selection&& other) noexcept;

    void add(const std::weak_ptr<Point>& point);

    const std::weak_ptr<Point>& operator[](std::size_t index) const;

    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;
};