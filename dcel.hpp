#pragma once

#include "point.hpp"
#include <memory>

class HalfEdge;
class Face;

class Vertex {
public:
    Point loc;
    // int idx = -1; // to remove
    std::shared_ptr<HalfEdge> edge;  // one of the leaving edges

    Vertex(double x, double y) : loc(x, y) {}
    Vertex(const Point& pt) : loc(pt) {}

    friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex);
};

class HalfEdge {
public:
    std::shared_ptr<Vertex> org;
    std::shared_ptr<HalfEdge> twin, prev, next;
    std::shared_ptr<Face> face;

    static std::shared_ptr<HalfEdge> newEdge(std::shared_ptr<Vertex> orgVertex, std::shared_ptr<Vertex> dstVertex);
    static void setNext(std::shared_ptr<HalfEdge> from, std::shared_ptr<HalfEdge> to);

    friend std::ostream& operator<<(std::ostream& os, const HalfEdge& edge);
};

class Face {
public:
    std::shared_ptr<HalfEdge> edge;  // one of the incident edges

    static std::shared_ptr<Face> newFace(std::shared_ptr<HalfEdge> startEdge);
    bool inFace(const Point& pt);

    friend std::ostream& operator<<(std::ostream& os, const Face& face);
};