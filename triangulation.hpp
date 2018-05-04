#pragma once

#include "dcel.hpp"
#include "convex_hull.hpp"

class Triangulation {
public:
    std::vector<Point> points;

    void run();
    void write(const std::string& fileName, bool debug = false);

private:
    std::vector<std::shared_ptr<Vertex>> vertices;
    std::shared_ptr<Face> outerFace;
};