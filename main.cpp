#include "triangulation.hpp"
#include <random>

int main() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0, 1000);
    std::vector<Point> points;
    std::unordered_map<Point, int> existedPoints;
    while (points.size() < 10) {
        Point pt(distribution(generator), distribution(generator));
        if (existedPoints.find(pt) == existedPoints.end()) {
            existedPoints.emplace(pt, points.size());
            points.push_back(pt);
        }
    }
    Triangulation trian;
    trian.points = points;
    trian.run();
    trian.write("test" + std::to_string(points.size()) + ".txt", true);

    return 0;
}