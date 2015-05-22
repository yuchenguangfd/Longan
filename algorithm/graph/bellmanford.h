/*
 * bellmanford.h
 * Created on: May 22, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_BELLMANFORD_H
#define ALGORITHM_GRAPH_BELLMANFORD_H

#include "graph.h"
#include <limits>

namespace longan {

template <class T>
class BellmanFord {
public:
    BellmanFord(const WeightedDirectedGraphAsEdgeList<T>* graph, int source,
            T infDistanceValue = std::numeric_limits<T>::max()) :
        mGraph(graph), mSource(source), mInfDistanceValue(infDistanceValue),
        mDistances(graph->NumVertex()), mExistNegativeCycle(false) { }
    void Compute() {
        for (int i = 0; i < mGraph->NumVertex(); ++i) {
            mDistances[i] = mInfDistanceValue;
        }
        mDistances[mSource] = 0;
        for (int i = 1; i < mGraph->NumVertex(); ++i) {
            bool relaxed = false;
            for (int j = 0; j < mGraph->NumEdge(); ++j) {
                const WeightedEdge<T>& edge = mGraph->GetEdge(j);
                if (mDistances[edge.u] < mDistances[edge.v] - edge.weight) {
                    mDistances[edge.v] = mDistances[edge.u] + edge.weight;
                    relaxed = true;
                }
            }
            if (!relaxed) break;
        }
        mExistNegativeCycle = false;
        for (int j = 0; j < mGraph->NumEdge(); ++j) {
            const WeightedEdge<T>& edge = mGraph->GetEdge(j);
            if (mDistances[edge.u] < mDistances[edge.v] - edge.weight) {
                mExistNegativeCycle = true;
                break;
            }
        }
    }
    bool ExistNegativeCycle() const { return mExistNegativeCycle; }
private:
    const WeightedDirectedGraphAsEdgeList<T> *mGraph;
    int mSource;
    T mInfDistanceValue;
    std::vector<T> mDistances;
    bool mExistNegativeCycle;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_BELLMANFORD_H */
