#include "triangulation.hpp"
#include <unordered_map>
#include <fstream>
#include <iostream>

void Triangulation::run() {
    // calculate bounding triangle
    double lx = points[0].x, ly = points[0].y, hx = lx, hy = ly;
    for (const auto& pt : points) {
        if (pt.x < lx) lx = pt.x;
        else if (pt.x > hx) hx = pt.x;
        if (pt.y < ly) ly = pt.y;
        else if (pt.y > hy) hy = pt.y;
    }
    double height = hy - ly, width = hx - lx;
    constexpr double margin = 1;
    vertices.push_back(std::make_shared<Vertex>(lx - height - margin, ly - margin));
    vertices.push_back(std::make_shared<Vertex>(hx + height + margin, ly - margin));
    vertices.push_back(std::make_shared<Vertex>((lx + hx) / 2, hy + width / 2 + margin)); // CCW
    
    // init DCEL
    auto face = std::make_shared<Face>();
    for (int i = 0; i < 3; ++i) {
        auto& edge = vertices[i]->edge;
        edge = std::make_shared<HalfEdge>();
        edge->org = vertices[i];
        edge->face = face;
        edge->twin = std::make_shared<HalfEdge>();
        edge->twin->face = face;
        edge->twin->twin = vertices[i]->edge;
    }
    for (int i = 0; i < 3; ++i) {
        int j = (i==2) ? 0 : i + 1;
        auto edge = vertices[i]->edge;
        auto nextEdge = vertices[j]->edge;
        edge->twin->org = vertices[j];
        edge->next = nextEdge;
        nextEdge->prev = edge;
        edge->twin->prev = nextEdge->twin;
        nextEdge->twin->next = edge->twin;
    }
    face->edge = vertices[0]->edge;
    outerFace = std::make_shared<Face>();
    outerFace->edge = vertices[0]->edge->twin;
}

void Triangulation::write(const std::string& fileName, bool debug) {
    // loci
    std::vector<Point> loci;
    std::unordered_map<Point, int> locToIdxMap;
    for (const auto& pt : points) {
        locToIdxMap[pt] = loci.size();
        loci.push_back(pt);
    }
    for (const auto& vertex : vertices) {
        if (locToIdxMap.find(vertex->loc) == locToIdxMap.end()) {
            locToIdxMap[vertex->loc] = loci.size();
            loci.push_back(vertex->loc);
        }
    }

    // adj lists
    std::vector<std::vector<int>> adjLists(loci.size());
    for (const auto& vertex : vertices) {
        int idx = locToIdxMap[vertex->loc];
        auto edge = vertex->edge;
        do {
            int adjIdx = locToIdxMap[edge->twin->org->loc];
            adjLists[idx].push_back(adjIdx);
            edge = edge->twin->next;
        }
        while (edge != vertex->edge);
    }

    // write
    std::ofstream ofs(fileName);
    for (int i = 0; i < loci.size(); ++i) {
        ofs << i << " " << loci[i].x << " " << loci[i].y;
        for (auto adj : adjLists[i]) {
            ofs << " " << adj;
        }
        ofs << std::endl;
    }
    ofs.close();
}