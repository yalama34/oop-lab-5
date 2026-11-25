#pragma once

#include "point.hpp"

class Criterion {
public:
    virtual ~Criterion() = default;
    virtual bool matches(const Point& point) const = 0;
};