#include "dcel.hpp"

std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
    os << vertex.loc;
    return os;
}

std::shared_ptr<HalfEdge> HalfEdge::newEdge(std::shared_ptr<Vertex> orgVertex, std::shared_ptr<Vertex> dstVertex) {
    auto edge = std::make_shared<HalfEdge>();
    edge->twin = std::make_shared<HalfEdge>();
    edge->org = orgVertex;
    edge->twin->org = dstVertex;
    edge->twin->twin = edge;
    return edge;
}

void HalfEdge::setNext(std::shared_ptr<HalfEdge> from, std::shared_ptr<HalfEdge> to) {
    to->prev = from;
    from->next = to;
}

std::ostream& operator<<(std::ostream& os, const HalfEdge& edge) {
    os << *(edge.org) << " " << *(edge.twin->org);
    return os;
}

std::shared_ptr<Face> Face::newFace(std::shared_ptr<HalfEdge> startEdge) {
    auto face = std::make_shared<Face>();
    face->edge = startEdge;
    auto cur = startEdge;
    do {
        cur->face = face;
        cur = cur->next;
    }
    while (cur != startEdge);
    return face;
}

bool Face::inFace(const Point& pt) {
    auto cur = edge;
    do {
        if (cross(cur->org->loc, pt, cur->twin->org->loc) > 0) {
            return false;
        }
        cur = cur->next;
    }
    while (cur != edge);
    return true;
}

std::ostream& operator<<(std::ostream& os, const Face& face) {
    auto cur = face.edge;
    do {
        os << *(cur->org) << " ";
        cur = cur->next;
    }
    while (cur != face.edge);
    return os;
}