#pragma once

#include "point.hpp"
#include <memory>

class HalfEdge;
class Face;

class Vertex {
public:
    Point loc;
    std::shared_ptr<HalfEdge> edge;  // one of the leaving edges

    Vertex(double x, double y) : loc(x, y) {}
};

class HalfEdge {
public:
    std::shared_ptr<Vertex> org;
    std::shared_ptr<HalfEdge> twin, prev, next;
    std::shared_ptr<Face> face;
};

class Face {
public:
    std::shared_ptr<HalfEdge> edge;  // one of the incident edges
};