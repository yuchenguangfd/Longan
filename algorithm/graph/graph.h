/*
 * graph.h
 * Created on: Jan 12, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_GRAPH_H
#define ALGORITHM_GRAPH_GRAPH_H

#include "common/lang/defines.h"
#include <vector>
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

template <class T, class Alloc = std::allocator<T>>
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

class DirectedGraphAsAdjList {
public:
    DirectedGraphAsAdjList(int numVertex, int maxNumEdge = 0) :
        mHeads(numVertex) {
        mEdges.reserve(maxNumEdge);
        mNexts.reserve(maxNumEdge);
        for (int i = 0; i < numVertex; ++i) {
            mHeads[i] = NIL;
        }
    }
    virtual ~DirectedGraphAsAdjList() { }
    int NumVertex() const  { return mHeads.size(); }
    int NumEdge() const { return mEdges.size(); }
    void AddEdge(const Edge &edge)  {
        mNexts.push_back(mHeads[edge.u]);
        mEdges.push_back(edge.v);
        mHeads[edge.u] = mEdges.size() - 1;
    }
    class EdgeIterator {
    public:
        EdgeIterator(const DirectedGraphAsAdjList *graph, int vertex) :
            mGraph(graph), mCurrentPtr(graph->mHeads[vertex]) { }
        bool HasNext() { return mCurrentPtr != NIL; }
        int NextVertex() {
            int next = mGraph->mEdges[mCurrentPtr];
            mCurrentPtr = mGraph->mNexts[mCurrentPtr];
            return next;
        }
    private:
        const DirectedGraphAsAdjList *mGraph;
        int mCurrentPtr;
    };
    EdgeIterator GetEdgeIteraror(int vertex) const {
        return EdgeIterator(this, vertex);
    }
private:
    static const int NIL = -1;
    std::vector<int> mHeads;
    std::vector<int> mEdges;
    std::vector<int> mNexts;
};

template <class T>
class WeightedDirectedGraphAsEdgeList {
public:
    WeightedDirectedGraphAsEdgeList(int numVertex, int maxNumEdge = 0) :
        mNumVertex(numVertex) {
        mEdgeList.reserve(maxNumEdge);
    }
    int NumVertex() const { return mNumVertex; }
    int NumEdge() const { return mEdgeList.size(); }
    void AddEdge(int u, int v, T weight) { AddEdge(WeightedEdge<T>(u, v, weight)); }
    void AddEdge(const WeightedEdge<T>& edge) { mEdgeList.push_back(edge); }
    const WeightedEdge<T>& GetEdge(int i) const { return mEdgeList[i]; }
protected:
    int mNumVertex;
    std::vector<WeightedEdge<T>> mEdgeList;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_GRAPH_H */
