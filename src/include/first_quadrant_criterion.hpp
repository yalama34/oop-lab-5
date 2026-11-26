#pragma once

#include "criterion.hpp"

class FirstQuadrantCriterion: public Criterion {
public:
    bool matches(const Point& point) const override;
};