/*
 * bipartite.h
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_BIPARTITE_H
#define ALGORITHM_GRAPH_BIPARTITE_H

#include <algorithm>
#include <memory>
#include <cassert>

namespace longan {

template <class Alloc = std::allocator<bool>>
class Bipartite {
public:
    Bipartite(int numVertexLeft, int numVertexRight);
    ~Bipartite();
    int NumVertexLeft() const { return mNumVertexLeft; }
    int NumVertexRight() const { return mNumVertexRight; }
    void AddEdge(int vertexLeft, int vertexRight) {
        assert(vertexLeft >= 0 && vertexLeft < mNumVertexLeft);
        assert(vertexRight >= 0 && vertexRight < mNumVertexRight);
        mGraph[vertexLeft * mNumVertexRight + vertexRight] = true;
    }
    bool ExistEdge(int vertexLeft, int vertexRight) const {
        assert(vertexLeft >= 0 && vertexLeft < mNumVertexLeft);
        assert(vertexRight >= 0 && vertexRight < mNumVertexRight);
        return mGraph[vertexLeft * mNumVertexRight + vertexRight];
    }
private:
    int mNumVertexLeft;
    int mNumVertexRight;
    bool *mGraph;
};

template <class Alloc>
Bipartite<Alloc>::Bipartite(int numVertexLeft, int numVertexRight) :
    mNumVertexLeft(numVertexLeft),
    mNumVertexRight(numVertexRight),
    mGraph(nullptr) {
    int size = mNumVertexLeft * mNumVertexRight;
    Alloc alloc;
    mGraph = alloc.allocate(size);
    std::fill(mGraph, mGraph + size, false);
}

template <class Alloc>
Bipartite<Alloc>::~Bipartite() {
    int size = mNumVertexLeft * mNumVertexRight;
    Alloc alloc;
    alloc.deallocate(mGraph, size);
}

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_BIPARTITE_H */
