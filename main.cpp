#include "triangulation.hpp"
#include <unordered_set>
#include <random>
#include <chrono>

void generate() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0, 1000);
    for (int numPoints = 10; numPoints <= 1000000; numPoints *= 10) {
        for (int i = 0; i < 3; ++i) {
            std::ofstream ofs("points" + std::to_string(numPoints) + "_" + std::to_string(i) + ".txt");
            ofs << numPoints << std::endl;
            std::unordered_set<Point> existedPoints;
            for (int j = 0; j < numPoints;) {
                Point pt(distribution(generator), distribution(generator));
                if (existedPoints.find(pt) == existedPoints.end()) {
                    existedPoints.emplace(pt);
                    ++j;
                    ofs << pt.x << " " << pt.y << std::endl;
                }
            }
        }
    }
}

void run(std::string filename) {
    std::ifstream ifs(filename);
    if (ifs.fail()) {
        std::cout << "Cannot open " << filename << ", skip..." << std::endl;
        return;
    }
    
    int numPoints = 0;
    ifs >> numPoints;
    Triangulation trian;
    trian.points.resize(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        if (!(ifs >> trian.points[i].x >> trian.points[i].y)) {
            std::cout << "Incorrect input format in " << filename << ", skip..." << std::endl;
            return;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    trian.run();
    auto dur = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count();

    int begin = filename.find_last_of('/');
    int end = filename.find_last_of('.');
    std::string outputFilename = filename.substr(begin + 1, end - begin - 1) + "_output.txt";
    trian.write(outputFilename);

    std::cout   << "input=" << filename << ", #points=" << trian.points.size() << ", #edges=" << trian.numEdges
                << ", time=" << dur << ", output=" << outputFilename << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 3 && std::string(argv[1]) == "-f") {
        run(argv[2]);
    }
    else if (argc == 2 && std::string(argv[1]) == "-b") {
        for (int numPoints = 10; numPoints <= 1000000; numPoints *= 10) {
            for (int i = 0; i < 3; ++i) {
                run("input_points/points" + std::to_string(numPoints) + "_" + std::to_string(i) + ".txt");
            }
        }
    }
    else if (argc == 2 && std::string(argv[1]) == "-g") {
        generate();
    }
    else {
        std::cout << "Usage 1: ./triangulation -f <file_name>" << std::endl;
        std::cout << "Usage 2: ./triangulation -b" << std::endl;
        std::cout << "Usage 3: ./triangulation -g" << std::endl;
    }

    return 0;
}