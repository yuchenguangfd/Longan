/*
 * dijkstra.h
 * Created on: May 22, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_DIJKSTRA_H
#define ALGORITHM_GRAPH_DIJKSTRA_H

#include "graph.h"
#include <limits>

namespace longan {

template <class T, class Alloc = std::allocator<T>>
class Dijkstra {
public:
    Dijkstra(const WeightedDirectedGraphAsAdjMatrix<T, Alloc>* graph, int source) :
        mGraph(graph), mSource(source), mDistances(graph->NumVertex()), mStates(graph->NumVertex()) { }
    void Compute() {
        for (int u = 0; u < mGraph->NumVertex(); ++u) {
            mDistances[u] = mGraph->GetWeight(mSource, u);
            mStates[u] = false;
        }
        mDistances[mSource] = T();
        mStates[mSource] = true;
        for (int i = 1; i < mGraph->NumVertex(); ++i) {
            T min = std::numeric_limits<T>::max();
            int k;
            for (int j = 0; j < mGraph->NumVertex(); ++j) {
                if (!mStates[j] && min > mDistances[j]) {
                    min = mDistances[j];
                    k = j;
                }
            }
            mStates[k] = true;
            for (int j = 0; j < mGraph->NumVertex(); ++j) {
                if (!mStates[j] && mDistances[j] > mDistances[k] + mGraph->GetWeight(k, j)) {
                    mDistances[j] = mDistances[k] + mGraph->GetWeight(k, j);
                }
            }
        }
    }
    T Distance(int v) const { return mDistances[v]; }
private:
    const WeightedDirectedGraphAsAdjMatrix<T, Alloc> *mGraph;
    int mSource;
    std::vector<T, Alloc> mDistances;
    std::vector<bool, Alloc> mStates;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_DIJKSTRA_H */
