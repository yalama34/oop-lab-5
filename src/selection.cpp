#include <memory>
#include <utility>
#include <stdexcept>

#include "selection.hpp"

Selection::Selection(const Selection& other)
    : size_(other.size_),
      capacity_(other.capacity_) {
    if (capacity_ > 0) {
        data_ = std::make_unique<std::weak_ptr<Point>[]>(capacity_);

        for (std::size_t i = 0; i < size_; i++) {
            data_[i] = other.data_[i];
        }
    }
}

Selection& Selection::operator=(const Selection& other) {
    if (this != &other) {
        Selection temp(other);

        std::swap(data_, temp.data_);
        std::swap(size_, temp.size_);
        std::swap(capacity_, temp.capacity_);
    }

    return *this;
}

Selection::Selection(Selection&& other) noexcept
    : data_(std::move(other.data_)),
      size_(other.size_),
      capacity_(other.capacity_) {
    other.size_ = 0;
    other.capacity_ = 0;
}

Selection& Selection::operator=(Selection&& other) noexcept {
    if (this != &other) {
        size_ = other.size_;
        capacity_ = other.capacity_;

        data_ = std::move(other.data_);

        other.size_ = 0;
        other.capacity_ = 0;
    }

    return *this;
}

const std::weak_ptr<Point>& Selection::operator[](std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Invalid index");
    }

    return data_[index];
}

void Selection::reserve(std::size_t new_capacity) {
    if (new_capacity <= capacity_) {
        return;
    }

    auto new_data =
        std::make_unique<std::weak_ptr<Point>[]>(new_capacity);

    for (std::size_t i = 0; i < size_; i++) {
        new_data[i] = data_[i];
    }

    data_ = std::move(new_data);
    capacity_ = new_capacity;
}

void Selection::add(const std::weak_ptr<Point>& point) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }

    data_[size_] = std::move(point);
    size_++;
}

std::size_t Selection::size() const noexcept {
    return size_;
}

std::size_t Selection::capacity() const noexcept {
    return capacity_;
}

bool Selection::empty() const noexcept {
    return size_ == 0;
}
