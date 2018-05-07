#include "triangulation.hpp"

void Triangulation::run() {
    init();
    for (int i = 0; i < points.size(); ++i) {
        insertPoint(i);
    }
}

void Triangulation::init() {
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
    for (int i = 0; i < 3; ++i) {
        int j = (i==2) ? 0 : i + 1;
        auto& edge = vertices[i]->edge;
        edge = HalfEdge::newEdge(vertices[i], vertices[j]);
    }
    for (int i = 0; i < 3; ++i) {
        int j = (i==2) ? 0 : i + 1;
        auto edge = vertices[i]->edge;
        auto nextEdge = vertices[j]->edge;
        HalfEdge::setNext(edge, nextEdge);
        HalfEdge::setNext(nextEdge->twin, edge->twin);
    }
    auto face = Face::newFace(vertices[0]->edge);
    outerFace = Face::newFace(vertices[0]->edge->twin);

    // point - face mapping
    auto& facePtIdxes = facePts[face];
    ptFaces.resize(points.size());
    for (int i = 0; i < points.size(); ++i) {
        facePtIdxes.push_back(i);
        ptFaces[i] = face;
    }
}

void Triangulation::insertPoint(int ptIdx) {
    auto face = ptFaces[ptIdx];
    auto centerVextex = std::make_shared<Vertex>(points[ptIdx]);
    vertices.push_back(centerVextex);
    auto startEdge = face->edge;
    // init
    auto edge = startEdge;
    do {
        HalfEdge::setNext(edge, HalfEdge::newEdge(edge->twin->org, centerVextex));
        edge = edge->prev;
    }
    while (edge != startEdge);
    centerVextex->edge = startEdge->next->twin;
    // connect
    edge = startEdge;
    std::vector<std::shared_ptr<Face>> faces;
    do {
        auto prevEdge = edge->prev;
        HalfEdge::setNext(edge->next, edge->prev->next->twin);
        HalfEdge::setNext(edge->prev->next->twin, edge);
        faces.push_back(Face::newFace(edge));
        edge = prevEdge;
    }
    while (edge != startEdge);
    // divide points into sub faces (triangles)
    std::vector<std::vector<int>> pts(faces.size());
    for (int pt : facePts[face]) {
        for (int i = 0; i < faces.size(); ++i) {
            if (faces[i]->inFace(points[pt])) {
                ptFaces[pt] = faces[i];
                pts[i].push_back(pt);
            }
        }
    }
    for (int i = 0; i < faces.size(); ++i) {
        facePts[faces[i]] = pts[i];
    }
    facePts.erase(face);
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