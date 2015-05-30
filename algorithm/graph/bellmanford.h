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

template <class WeightType, class DistanceType>
class BellmanFord {
public:
    BellmanFord(const WeightedDirectedGraphAsEdgeList<WeightType>* graph, int source) :
        mGraph(graph), mSource(source), mDistances(graph->NumVertex()), mExistNegativeCycle(false) { }
    virtual ~BellmanFord() { }
    void Compute() {
        DistanceType initDist = GetInitDistance();
        for (int i = 0; i < mGraph->NumVertex(); ++i) {
            mDistances[i] = initDist;
        }
        mDistances[mSource] = GetSourceDistance();
        for (int i = 1; i < mGraph->NumVertex(); ++i) {
            bool relaxed = false;
            for (int j = 0; j < mGraph->NumEdge(); ++j) {
                if (Relax(mGraph->GetEdge(j))) {
                    relaxed = true;
                }
            }
            if (!relaxed) break;
        }
        mExistNegativeCycle = false;
        for (int j = 0; j < mGraph->NumEdge(); ++j) {
            if (Relax(mGraph->GetEdge(j))) {
                mExistNegativeCycle = true;
                break;
            }
        }
    }
    virtual DistanceType GetInitDistance() const {
        return std::numeric_limits<DistanceType>::max();
    }
    virtual DistanceType GetSourceDistance() const {
        return 0;
    }
    virtual bool Relax(const WeightedEdge<WeightType>& edge) {
        if (mDistances[edge.u] < mDistances[edge.v] - edge.weight) {
            mDistances[edge.v] = mDistances[edge.u] + edge.weight;
            return true;
        } else {
            return false;
        }
    }
    bool ExistNegativeCycle() const { return mExistNegativeCycle; }
    DistanceType Distance(int v) const { return mDistances[v]; }
protected:
    const WeightedDirectedGraphAsEdgeList<WeightType> *mGraph;
    int mSource;
    std::vector<DistanceType> mDistances;
    bool mExistNegativeCycle;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_BELLMANFORD_H */
