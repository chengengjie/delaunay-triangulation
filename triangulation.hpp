#pragma once

#include "dcel.hpp"

#include <vector>
#include <unordered_map>
#include <fstream>

class Triangulation {
public:
    std::vector<Point> points;

    void run();
    void write(const std::string& fileName, bool debug = false);

private:
    std::vector<std::shared_ptr<Face>> ptFaces;             // point idx -> face
    std::unordered_map<std::shared_ptr<Face>,std::vector<int>> facePts;  // face idx -> point idx
    std::shared_ptr<Face> outerFace;
    std::vector<std::shared_ptr<Vertex>> vertices;

    void init();
    void insertPoint(int ptIdx);
};