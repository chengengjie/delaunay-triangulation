#include "convex_hull.hpp"
#include <algorithm>
#include <iostream>

void ConvexHull::run() {
    // initialize
    int n = points.size(), k = 0;
    if (n <= 3) return;
    std::vector<Point> hull(2*n);

    // sort points lexicographically
    std::sort(points.begin(), points.end(), [](const Point& lhs, const Point& rhs){
        return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    });

    // build lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(hull[k-2], hull[k-1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    // build upper hull
    int lowerSize = k;
    for (int i = n-2; i >= 0; --i) {
        while (k > lowerSize && cross(hull[k-2], hull[k-1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    // finalize
    hull.resize(k-1);
    points = move(hull);
}