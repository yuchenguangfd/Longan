/*
 * bipartite_max_match.h
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_BIPARTITE_MAX_MATCH_H
#define ALGORITHM_GRAPH_BIPARTITE_MAX_MATCH_H

#include "bipartite.h"
#include <vector>

namespace longan {

template <class Alloc = std::allocator<bool>>
class BipartiteMaxMatch {
public:
    struct MatchPair {
        int vertexLeft;
        int vertexRight;
        MatchPair(int s, int t) : vertexLeft(s), vertexRight(t) { }
    };
    int ComputeMaxMatch(const Bipartite<Alloc>& bipartite, std::vector<MatchPair> *maxMatch = nullptr);
private:
    bool Find(int x);
private:
    const Bipartite<Alloc> *mBipartite;
    std::vector<int> mLink;
    std::vector<bool> mUsed;
};

template <class Alloc>
int BipartiteMaxMatch<Alloc>::ComputeMaxMatch(const Bipartite<Alloc>& bipartite, std::vector<MatchPair> *maxMatch) {
    mBipartite = &bipartite;
    mLink.resize(mBipartite->NumVertexRight());
    std::fill(mLink.begin(), mLink.end(), -1);
    mUsed.resize(mBipartite->NumVertexRight());
    int result = 0;
    for (int i = 0; i < mBipartite->NumVertexLeft(); ++i) {
        std::fill(mUsed.begin(), mUsed.end(), 0);
        if (Find(i)) {
            ++result;
        }
    }
    return result;
}

template <class Alloc>
bool BipartiteMaxMatch<Alloc>::Find(int x) {
    for(int i = 0; i < mBipartite->NumVertexRight(); ++i) {
        if (!mUsed[i] && mBipartite->ExistEdge(x, i)) {
            mUsed[i] = true;
            if(mLink[i] == -1 || Find(mLink[i])) {
                mLink[i] = x;
                return true;
            }
        }
    }
    return false;
}

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_BIPARTITE_MAX_MATCH_H */
