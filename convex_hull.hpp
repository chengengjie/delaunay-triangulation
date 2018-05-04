#pragma once

#include "point.hpp"
#include <vector>

class ConvexHull {
public:
    void run();
    std::vector<Point> points;
};