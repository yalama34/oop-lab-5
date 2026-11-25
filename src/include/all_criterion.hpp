#pragma once

#include "criterion.hpp"

class AllCriterion: Criterion {
public:
    bool matches(const Point& point) const override;
};