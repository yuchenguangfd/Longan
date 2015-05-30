/*
 * topo_sort.h
 * Created on: May 29, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_TOPO_SORT_H
#define ALGORITHM_GRAPH_TOPO_SORT_H

#include "graph.h"

namespace longan {

template <class GraphType>
class TopoSort {
public:
    TopoSort(const GraphType* graph) :
        mGraph(graph), mColors(graph->NumVertex()), mSequence(graph->NumVertex()),
        mIsDAG(false), mCurrentSequenceIndex(0) { }
    void Compute() {
        for (int u = 0; u < mGraph->NumVertex(); ++u) {
            mColors[u] = ColorWhite;
        }
        mIsDAG = true;
        mCurrentSequenceIndex = mGraph->NumVertex();
        for (int u = 0; mIsDAG && u < mGraph->NumVertex(); ++u) {
            if (mColors[u] == ColorWhite) {
                DFSVisit(u);
            }
        }
    }
    int SequenceAt(int i) const { return mSequence[i]; }
    bool IsDAG() const { return mIsDAG; }
private:
    void DFSVisit(int u) {
        mColors[u] = ColorGray;
        typename GraphType::EdgeIterator iter = mGraph->GetEdgeIteraror(u);
        while (iter.HasNext()) {
            int v = iter.NextVertex();
            if (mColors[v] == ColorWhite) {
                DFSVisit(v);
                if (!mIsDAG) return;
            } else if (mColors[v] == ColorGray) {
                mIsDAG = false;
                return;
            }
        }
        mColors[u] = ColorBlack;
        mSequence[--mCurrentSequenceIndex] = u;
    }
private:
    const GraphType* mGraph;
    enum Color {
        ColorWhite, ColorBlack, ColorGray
    };
    std::vector<int> mColors;
    std::vector<int> mSequence;
    int mCurrentSequenceIndex;
    bool mIsDAG;
};

} //~ namespace longan

#endif // ALGORITHM_GRAPH_TOPO_SORT_H
