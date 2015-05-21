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
        int *end = begin + mNumVertex;
        for (; begin != end; ++begin) {
            degree += *begin;
        }
        return degree;
    }
private:
    int mNumVertex;
    int *mData;
};

template <class Alloc = std::allocator<int>>
class DirectedGraphAsAdjMatrix {
public:
    DirectedGraphAsAdjMatrix(int numVertex) : mNumVertex(numVertex) {
        int size = numVertex * numVertex;
        Alloc alloc;
        mData = alloc.allocate(size);
        std::fill(mData, mData + size, 0);
    }
    ~DirectedGraphAsAdjMatrix() {
        int size = mNumVertex * mNumVertex;
        Alloc alloc;
        alloc.deallocate(mData, size);
    }
    int NumVertex() const { return mNumVertex; }
    bool ExistEdge(int u, int v) const { return mData[u * mNumVertex + v] > 0; }
    bool ExistEdge(const Edge& e) const { return mData[e.u * mNumVertex + e.v] > 0; }
    void AddEdge(int u, int v) { ++mData[u * mNumVertex + v]; }
    void AddEdge(const Edge& e) { ++mData[e.u * mNumVertex + e.v]; }
    void RemoveEdge(int u, int v) { --mData[u * mNumVertex + v]; }
    void RemoveEdge(const Edge& e) { --mData[e.u * mNumVertex + e.v]; }
    void RemoveEdgesFromVertex(int u) {
        for (int v = 0; v < mNumVertex; ++v) {
            mData[u * mNumVertex + v] = 0;
        }
    }
    void RemoveEdgesToVertex(int v) {
        for (int u = 0; u < mNumVertex; ++u) {
           mData[u * mNumVertex + v] = 0;
        }
    }
    int DegreeOutOfVertex(int u) const {
        int degree = 0;
        int *begin = mData + u * mNumVertex;
        int *end = begin + mNumVertex;
        for (; begin != end; ++begin) {
            degree += *begin;
        }
        return degree;
    }
    int DegreeInOfVertex(int v) const {
        int degree = 0;
        int *begin = mData + v;
        int *end = begin + mNumVertex*mNumVertex;
        for (; begin != end; begin += mNumVertex) {
            degree += *begin;
        }
        return degree;
    }
private:
    int mNumVertex;
    int *mData;
};

template <class T, class Alloc = std::allocator<T> >
class WeightedDirectedGraphAsAdjMatrix {
public:
    WeightedDirectedGraphAsAdjMatrix(int numVertex, T initWeight = T()) : mNumVertex(numVertex) {
        int size = mNumVertex * mNumVertex;
        Alloc alloc;
        mData = alloc.allocate(size);
        for (int i = 0; i < size; ++i) {
            mData[i] = initWeight;
        }
    }
    virtual ~WeightedDirectedGraphAsAdjMatrix() {
        int size = mNumVertex * mNumVertex;
        Alloc alloc;
        alloc.deallocate(mData, size);
    }
    int NumVertex() const { return mNumVertex; }
    T GetWeight(int u, int v) const { return mData[u * mNumVertex + v]; }
    virtual void AddEdge(int u, int v, T weight) { mData[u * mNumVertex + v] = weight; }
protected:
    int mNumVertex;
    T *mData;
};

template <class Alloc = std::allocator<int>>
class DirectedGraphAsAdjList {
public:
    DirectedGraphAsAdjList(int numVertex, int maxNumEdge) : mNumVertex(numVertex), mMaxNumEdge(maxNumEdge),
            mNumEdge(0) {
        Alloc alloc;
        mHeads = alloc.allocate(mNumVertex);
        std::fill(mHeads, mHeads + mNumVertex, NIL);
        mEdges = alloc.allocate(mMaxNumEdge);
        mNexts = alloc.allocate(mMaxNumEdge);
    }
    ~DirectedGraphAsAdjList() {
        Alloc alloc;
        alloc.deallocate(mHeads, mNumVertex);
        alloc.deallocate(mEdges, mMaxNumEdge);
        alloc.deallocate(mNexts, mMaxNumEdge);
    }
    int NumVertex() const  { return mNumVertex; }
    int NumEdge() const { return mNumEdge; }
    void AddEdge(const Edge &edge)  {
        mEdges[mNumEdge] = edge.v;
        mNexts[mNumEdge] = mHeads[edge.u];
        mHeads[edge.u] = mNumEdge;
        ++mNumEdge;
    }
    int DegreeOutOfVertex(int u) const {
        int p = mHeads[u];
        int count = 0;
        while (p != NIL) {
            p = mNexts[p];
            ++count;
        }
        return count;
    }

    class EdgeIterator {
    public:
        EdgeIterator(const DirectedGraphAsAdjList *graph, int vertex) : mGraph(graph), mCurrPtr(graph->mHeads[vertex]) { }
        bool HasNext() { return mCurrPtr != NIL; }
        int Next() {
            int v = mGraph->mEdges[mCurrPtr];
            mCurrPtr = mGraph->mNexts[mCurrPtr];
            return v;
        }
    private:
        const DirectedGraphAsAdjList *mGraph;
        int mCurrPtr;
    };

    EdgeIterator GetEdgeIteraror(int vertex) const {
        return EdgeIterator(this, vertex);
    }
private:
    static const int NIL = -1;
    int mNumVertex;
    int mNumEdge;
    int mMaxNumEdge;
    int *mHeads;
    int *mEdges;
    int *mNexts;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_GRAPH_H */
