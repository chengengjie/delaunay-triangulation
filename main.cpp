#include "triangulation.hpp"
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>

int main() {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 10);
    std::vector<Point> points;
    std::unordered_map<Point, int> existedPoints;
    while (points.size() < 10) {
        Point pt(distribution(generator), distribution(generator));
        if (existedPoints.find(pt) == existedPoints.end()) {
            existedPoints.emplace(pt, points.size());
            points.push_back(pt);
        }
    }
    ConvexHull ch;
    ch.points = points;
    ch.run();
    std::vector<int> pointIndexes(ch.points.size());
    for (int i = 0; i < ch.points.size(); ++i) {
        pointIndexes[i] = existedPoints[ch.points[i]];
    }
    std::vector<std::vector<int>> adjLists(points.size());
    for (int i = 0; i < ch.points.size(); ++i) {
        int j = (i==((int)ch.points.size()-1)) ? 0 : i+1;
        adjLists[pointIndexes[i]].push_back(pointIndexes[j]);
        adjLists[pointIndexes[j]].push_back(pointIndexes[i]);
    }
    std::ofstream ofs("test.txt");
    for (int i = 0; i < points.size(); ++i) {
        ofs << i << " " << points[i].x << " " << points[i].y;
        for (auto adj : adjLists[i]) {
            ofs << " " << adj;
        }
        ofs << std::endl;
    }

    return 0;
}