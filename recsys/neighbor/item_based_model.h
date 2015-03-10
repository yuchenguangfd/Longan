/*
 * item_based_model.h
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H
#define RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H

#include "item_based_util.h"
#include "recsys/base/similarity_matrix.h"
#include <vector>
#include <string>

namespace longan {

namespace ItemBased {

class NeighborItem {
public:
    NeighborItem(float sim, float rating) : mSimilarity(sim), mRating(rating) { }
    float Similarity() const { return mSimilarity; }
    float Rating() const { return mRating; }
private:
    float mSimilarity;
    float mRating;
};

inline bool operator< (const NeighborItem& lhs, const NeighborItem& rhs) {
    return lhs.Similarity() < rhs.Similarity();
}

class Model : public SimilarityMatrix {
public:
    Model(const Parameter *param, int numItem);
    int NumItem() const { return mSize; }
    const Parameter* GetParameter() const { return mParameter; }
    void Save(const std::string& filename);
    void Load(const std::string& filename);
private:
    const Parameter *mParameter;
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H */
