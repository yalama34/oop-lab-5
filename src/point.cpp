#include "point.hpp"

Point::Point(double x, double y): x_(x), y_(y) {
    alive_count_++;
}

Point::~Point() {
    alive_count_--;
}

double Point::x() const {return x_;}
double Point::y() const {return y_;}

int Point::aliveCount() noexcept {return alive_count_;}