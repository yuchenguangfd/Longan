/*
 * weighted_bipartite_max_match.h
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_WEIGHTED_BIPARTITE_MAX_MATCH_H
#define ALGORITHM_GRAPH_WEIGHTED_BIPARTITE_MAX_MATCH_H

#include "weighted_bipartite.h"
#include <algorithm>
#include <limits>
#include <vector>

namespace longan {

struct BipartiteMatchPair {
    int vertexLeft;
    int vertexRight;
    BipartiteMatchPair(int s, int t) : vertexLeft(s), vertexRight(t) { }
};

template <class T, class Alloc = std::allocator<T> >
class WeightedBipartiteMaxMatch {
public:
    // Kuhn–Munkres algorithm
    T ComputeMaxMatch(const WeightedBipartite<T, Alloc>& bipartite, std::vector<BipartiteMatchPair> *maxMatch);
private:
    void Init();
    bool Find(int x);
private:
    const WeightedBipartite<T, Alloc> *mBipartite;
    std::vector<int> mLinkY;
    std::vector<T> mPotentialLeft;
    std::vector<T> mPotentialRight;
    std::vector<T> mSlack;
    std::vector<bool> mVistedLeft;
    std::vector<bool> mVistedRight;
};

template <class T, class Alloc>
T WeightedBipartiteMaxMatch<T, Alloc>::ComputeMaxMatch(const WeightedBipartite<T, Alloc>& bipartite,
        std::vector<BipartiteMatchPair> *maxMatch) {
    mBipartite = &bipartite;
    Init();
    for(int x = 0; x < mBipartite->NumVertexLeft(); ++x) {
        std::fill(mSlack.begin(), mSlack.end(), std::numeric_limits<T>::max());
        while(true) {
            std::fill(mVistedLeft.begin(), mVistedLeft.end(), false);
            std::fill(mVistedRight.begin(), mVistedRight.end(), false);
            if(Find(x)) break;
            T d = std::numeric_limits<T>::max();
            for(int i = 0; i < mBipartite->NumVertexRight(); ++i) {
                if (!mVistedRight[i] && d > mSlack[i])
                    d = mSlack[i];
            }
            for(int i = 0; i < mBipartite->NumVertexLeft(); ++i) {
                if (mVistedLeft[i]) {
                    mPotentialLeft[i] -= d;
                }
            }
            for(int i = 0; i < mBipartite->NumVertexRight(); ++i) {
                if (mVistedRight[i]) {
                    mPotentialRight[i] += d;
                } else {
                    mSlack[i] -= d;
                }
            }
        }
    }
    if (maxMatch == nullptr) {
        T result = T();
        for(int i = 0; i < mBipartite->NumVertexRight(); ++i) {
            if(mLinkY[i] > -1) {
                result += mBipartite->GetWeight(mLinkY[i], i);
            }
        }
        return result;
    } else {
        T result = T();
        maxMatch->clear();
        for(int i = 0; i < mBipartite->NumVertexRight(); ++i) {
            if(mLinkY[i] > -1) {
                maxMatch->push_back(BipartiteMatchPair(mLinkY[i], i));
                result += mBipartite->GetWeight(mLinkY[i], i);
            }
        }
        std::sort(maxMatch->begin(), maxMatch->end(),
                [](const BipartiteMatchPair& lhs, const BipartiteMatchPair& rhs)->bool {
            return lhs.vertexLeft < rhs.vertexLeft;
        });
        return result;
    }
}

template <class T, class Alloc>
void WeightedBipartiteMaxMatch<T, Alloc>::Init() {
    mLinkY.resize(mBipartite->NumVertexRight());
    std::fill(mLinkY.begin(), mLinkY.end(), -1);
    mPotentialRight.resize(mBipartite->NumVertexRight());
    std::fill(mPotentialRight.begin(), mPotentialRight.end(), T());
    mPotentialLeft.resize(mBipartite->NumVertexLeft());
    for(int i = 0; i < mBipartite->NumVertexLeft(); ++i) {
        mPotentialLeft[i] = mBipartite->GetWeight(i, 0);
        for(int j = 1; j < mBipartite->NumVertexRight(); ++j) {
            mPotentialLeft[i] = std::max(mPotentialLeft[i], mBipartite->GetWeight(i, j));
        }
    }
    mSlack.resize(mBipartite->NumVertexRight());
    mVistedLeft.resize(mBipartite->NumVertexLeft());
    mVistedRight.resize(mBipartite->NumVertexRight());
}

template <class T, class Alloc>
bool WeightedBipartiteMaxMatch<T, Alloc>::Find(int x) {
    mVistedLeft[x] = true;
    for(int y = 0; y < mBipartite->NumVertexRight(); ++y) {
        if(mVistedRight[y]) continue;
        int delta = mPotentialLeft[x] + mPotentialRight[y] - mBipartite->GetWeight(x, y);
        if(delta == 0) {
            mVistedRight[y] = true;
            if(mLinkY[y]==-1 || Find(mLinkY[y])) {
                mLinkY[y] = x;
                return true;
            }
        }
        else if(mSlack[y] > delta)
            mSlack[y] = delta;
    }
    return false;
}

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_WEIGHTED_BIPARTITE_MAX_MATCH_H */
