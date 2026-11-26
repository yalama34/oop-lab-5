#include "first_quadrant_criterion.hpp"

bool FirstQuadrantCriterion::matches(const Point& point) const{
    return point.x() > 0 && point.y() > 0; 
}
