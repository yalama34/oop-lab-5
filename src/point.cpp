#include "point.hpp"

Point::Point(double x, double y): x_(x), y_(y) {
    alive_count_++;
}

Point::Point(const Point& other)
    : x_(other.x_), y_(other.y_) {
    alive_count_++;
}

Point::~Point() {
    alive_count_--;
}

double Point::x() const {return x_;}
double Point::y() const {return y_;}

bool Point::operator<(const Point& other) const {
    return (x_ < other.x_ && y_ < other.y_);
}

bool Point::operator==(const Point& other) const {
    return (x_ == other.x_ && y_ < other.y_);
}

int Point::aliveCount() noexcept {return alive_count_;}

int Point::alive_count_ = 0;