#pragma once

#include <functional>
#include <iostream>

class Point {
public:
    double x, y;

    Point(double xVal = 0, double yVal = 0) : x(xVal), y(yVal) {}

    double squareSum() const {
        return x * x + y * y;
    }

    bool operator==(const Point& rhs) const { return x == rhs.x && y == rhs.y; }
    friend std::ostream& operator<<(std::ostream& os, const Point& pt) {
        os << "(x=" << pt.x << ", y=" << pt.y << ")";
        return os;
    }
};

namespace std {

// hash function for Point
template <>
struct hash<Point> {
    std::size_t operator()(const Point& pt) const {
        return (std::hash<double>()(pt.x) ^ std::hash<double>()(pt.y));
    }
};

}

// 2D cross product of AB and BC vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if ABC makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
inline double cross(const Point& ptA, const Point& ptB, const Point& ptC) {
    return (ptB.x - ptA.x) * (ptC.y - ptA.y) - (ptB.y - ptA.y) * (ptC.x - ptA.x);
}

// Returns true if D is inside the circle ABC
inline bool inCircle(const Point& ptA, const Point& ptB, const Point& ptC, const Point& ptD) {
    return  ptA.squareSum() * cross(ptB, ptC, ptD) - 
            ptB.squareSum() * cross(ptA, ptC, ptD) +
            ptC.squareSum() * cross(ptA, ptB, ptD) -
            ptD.squareSum() * cross(ptA, ptB, ptC) > 0;
}