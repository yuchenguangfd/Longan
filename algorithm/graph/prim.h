/*
 * prim.h
 * Created on: Jun 2, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_PRIM_H
#define ALGORITHM_GRAPH_PRIM_H

#include "graph.h"
#include <limits>

namespace longan {

template <class GraphType, class WeightType>
class Prim {
public:
	Prim(const GraphType *graph) : mGraph(graph), mDistances(graph->NumVertex()), mStates(graph->NumVertex()),
		mTotalWeight(0) { }
	void Compute() {
		int source = 0;
		for (int u = 0; u < mGraph->NumVertex(); ++u) {
			mDistances[u] = mGraph->GetWeight(source, u);
			mStates[u] = false;
		}
		mDistances[source] = 0;
		mStates[source] = true;
		mTotalWeight = 0;
		for (int i = 1; i < mGraph->NumVertex(); ++i) {
			WeightType min = std::numeric_limits<WeightType>::max();
			int k;
			for (int j = 0; j < mGraph->NumVertex(); ++j) {
				if (!mStates[j] && min > mDistances[j]) {
					min = mDistances[j];
					k = j;
				}
			}
			mStates[k] = true;
			mDistances[k] = min;
			mTotalWeight += min;
			for (int j = 0; j < mGraph->NumVertex(); ++j) {
				if (!mStates[j] && mGraph->GetWeight(k, j) < mDistances[j]) {
					mDistances[j] = mGraph->GetWeight(k, j);
				}
			}
		}
	}
	WeightType TotalWeight() const {
		return mTotalWeight;
	}
private:
	const GraphType *mGraph;
	std::vector<WeightType> mDistances;
	std::vector<bool> mStates;
	WeightType mTotalWeight;
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_PRIM_H */
