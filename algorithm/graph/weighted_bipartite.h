/*
 * weighted_bipartite.cpp
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_WEIGHTED_BIPARTITE_H
#define ALGORITHM_GRAPH_WEIGHTED_BIPARTITE_H

#include <memory>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T> >
class WeightedBipartite {
public:
    WeightedBipartite(int numVertexLeft, int numVertexRight);
    ~WeightedBipartite();
    int NumVertexLeft() const {
        return mNumVertexLeft;
    }
    int NumVertexRight() const {
        return mNumVertexRight;
    }
    void AddWeight(int vertexLeft, int vertexRight, T weight);
    T GetWeight(int vertexLeft, int vertexRight) const;
private:
    int mNumVertexLeft;
    int mNumVertexRight;
    T *mWeight;
};

template <class T, class Alloc>
WeightedBipartite<T, Alloc>::WeightedBipartite(int numVertexLeft, int numVertexRight) :
    mNumVertexLeft(numVertexLeft),
    mNumVertexRight(numVertexRight),
    mWeight(nullptr){
    int size = mNumVertexLeft * mNumVertexRight;
    Alloc alloc;
    mWeight = alloc.allocate(size);
    for (int i = 0; i < size; ++i) {
        alloc.construct(&mWeight[i], T());
    }
}

template <class T, class Alloc>
WeightedBipartite<T, Alloc>::~WeightedBipartite() {
    int size = mNumVertexLeft * mNumVertexRight;
    Alloc alloc;
    alloc.deallocate(mWeight, size);
}

template <class T, class Alloc>
void WeightedBipartite<T, Alloc>::AddWeight(int vertexLeft, int vertexRight, T weight) {
    assert(vertexLeft >= 0 && vertexLeft < mNumVertexLeft);
    assert(vertexRight >= 0 && vertexRight < mNumVertexRight);
    mWeight[vertexLeft * mNumVertexRight + vertexRight] += weight;
}

template <class T, class Alloc>
T WeightedBipartite<T, Alloc>::GetWeight(int vertexLeft, int vertexRight) const {
    assert(vertexLeft >= 0 && vertexLeft < mNumVertexLeft);
    assert(vertexRight >= 0 && vertexRight < mNumVertexRight);
    return mWeight[vertexLeft * mNumVertexRight + vertexRight];
}

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_WEIGHTED_BIPARTITE_H */
