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
    constexpr double margin = 10;
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
    auto centerVertex = std::make_shared<Vertex>(points[ptIdx]);
    // centerVertex->idx = ptIdx;
    vertices.push_back(centerVertex);
    // store inner edges
    std::vector<std::shared_ptr<HalfEdge>> innerEdges;
    auto edge = face->edge;
    do {
        innerEdges.push_back(edge);
        edge = edge->next;
    }
    while (edge != face->edge);
    // init
    for (auto edge : innerEdges) {
        HalfEdge::setNext(edge, HalfEdge::newEdge(edge->twin->org, centerVertex));
    }
    centerVertex->edge = face->edge->next->twin;
    // connect
    std::vector<std::shared_ptr<Face>> faces;
    for (auto edge : innerEdges) {
        HalfEdge::setNext(edge->next, edge->prev->next->twin);
        HalfEdge::setNext(edge->prev->next->twin, edge);
        faces.push_back(Face::newFace(edge));
    }
    // divide points into sub faces (triangles)
    std::vector<std::vector<int>> pts(faces.size());
    rebucket(faces, facePts[face], pts);
    facePts.erase(face);
    for (int i = 0; i < faces.size(); ++i) {
        facePts[faces[i]] = pts[i];
    }
    // swap test
    for (auto edge : innerEdges) {
        swapTest(edge);
    }
}

void Triangulation::rebucket(const std::vector<std::shared_ptr<Face>>& faces, const std::vector<int>& inputPts, std::vector<std::vector<int>>& pts) {
    for (int pt : inputPts) {
        for (int i = 0; i < faces.size(); ++i) {
            if (faces[i]->inFace(points[pt])) {
                ptFaces[pt] = faces[i];
                pts[i].push_back(pt);
            }
        }
    }
}

void Triangulation::swapTest(std::shared_ptr<HalfEdge> edge) {
    if (edge->twin->face == outerFace) {
        return;
    }
    // edge is AB
    auto edgePA = edge->prev;
    auto edgeDB = edge->twin->prev;
    if (inCircle(   edgePA->org->loc,       // P
                    edge->org->loc,         // A
                    edge->twin->org->loc,   // B
                    edgeDB->org->loc)) {    // D
        // add PD
        auto edgePD = HalfEdge::newEdge(edgePA->org, edgeDB->org); // PD
        HalfEdge::setNext(edgePA->prev, edgePD);        // B-P-D
        HalfEdge::setNext(edgePD->twin, edgePA);        // D-P-A
        HalfEdge::setNext(edgeDB->prev, edgePD->twin);  // A-D-P
        HalfEdge::setNext(edgePD, edgeDB);              // P-D-B
        // remove AB
        for (auto halfEdge : {edge, edge->twin}) {
            if (halfEdge->org->edge == halfEdge) {
                halfEdge->org->edge = halfEdge->twin->next;
            }
        }
        HalfEdge::setNext(edge->prev, edge->twin->next);    // P-A-D
        HalfEdge::setNext(edge->twin->prev, edge->next);    // D-A-P
        // overwrite faces
        std::vector<std::shared_ptr<Face>> oldFaces = {edge->face, edge->twin->face};
        std::vector<std::shared_ptr<Face>> faces;
        faces.push_back(Face::newFace(edgePD));
        faces.push_back(Face::newFace(edgePD->twin));
        // rebucket
        std::vector<std::vector<int>> pts(faces.size());
        for (auto oldFace : oldFaces) {
            rebucket(faces, facePts[oldFace], pts);
            facePts.erase(oldFace);
        }
        for (int i = 0; i < faces.size(); ++i) {
            facePts[faces[i]] = pts[i];
        }
        swapTest(edgePD->next);         // DB
        swapTest(edgePD->twin->prev);   // DA
    }
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
        if (debug || i < points.size()) {
            ofs << i << " " << loci[i].x << " " << loci[i].y;
            for (auto adj : adjLists[i]) {
                if (debug || adj < points.size()) {
                    ofs << " " << adj;
                }
            }
            ofs << std::endl;
        }
    }
    ofs.close();
}