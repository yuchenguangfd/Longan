/*
 * graph.h
 * Created on: Jan 12, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_GRAPH_H
#define ALGORITHM_GRAPH_GRAPH_H

#include "common/lang/defines.h"
#include <memory>
#include <algorithm>

namespace longan {

struct Edge {
    int u, v;
    Edge(int u, int v) : u(u), v(v) { }
};

template <class T>
struct WeightedEdge {
    int u, v;
    T weight;
    WeightedEdge(int u, int v, T weight) : u(u), v(v), weight(weight) { }
};

template <class Alloc = std::allocator<int>>
class UndirectedGraphAsAdjMatrix {
public:
    UndirectedGraphAsAdjMatrix(int numVertex) : mNumVertex(numVertex) {
        int size = numVertex * numVertex;
        Alloc alloc;
        mData = alloc.allocate(size);
        std::fill(mData, mData + size, 0);
    }
    ~UndirectedGraphAsAdjMatrix() {
        int size = mNumVertex * mNumVertex;
        Alloc alloc;
        alloc.deallocate(mData, size);
    }
    int NumVertex() const { return mNumVertex; }
    bool ExistEdge(int u, int v) const { return mData[u * mNumVertex + v] > 0; }
    bool ExistEdge(const Edge& e) const { return mData[e.u * mNumVertex + e.v] > 0; }
    void AddEdge(const Edge& e) {
        ++mData[e.u * mNumVertex + e.v];
        ++mData[e.v * mNumVertex + e.u];
    }
    void RemoveEdge(const Edge& e) {
        --mData[e.u * mNumVertex + e.v];
        --mData[e.v * mNumVertex + e.u];
    }
    void RemoveAllEdgesOfVertex(int v) {
        for (int u = 0; u < mNumVertex; ++u) {
            mData[u * mNumVertex + v] = 0;
            mData[v * mNumVertex + u] = 0;
        }
    }
    int DegreeOfVertex(int v) const {
        int degree = 0;
        int *begin = mData + v * mNumVertex;
        for (int i = 0; i < mNumVertex; ++i) {
            degree += begin[i];
        }
        return degree;
    }
private:
    int mNumVertex;
    int *mData;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_GRAPH_H */
