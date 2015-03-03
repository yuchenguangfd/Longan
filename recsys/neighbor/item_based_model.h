/*
 * item_based_model.h
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H
#define RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H

#include "item_based_util.h"
#include "common/util/running_statistic.h"
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

class ModelTrain {
public:
    ModelTrain(const Parameter *param, int numItem);
    int NumItem() const { return mNumItem; }
    const Parameter* GetParameter() const { return mParameter; }
    void Save(const std::string& filename);
    void PutSimilarity(int iid1, int iid2, float sim) {
        if (iid1 < iid2) {
            mSimMat[iid2][iid1] = sim;
        } else {
            mSimMat[iid1][iid2] = sim;
        }
    }
    float GetSimilarity(int iid1, int iid2) const {
        return (iid1 < iid2) ? mSimMat[iid2][iid1] : mSimMat[iid1][iid2];
    }
private:
    const Parameter *mParameter;
    int mNumItem;
    std::vector<std::vector<float>> mSimMat; // triangle matrix
};

class ModelPredict {
public:
    ModelPredict();
    int NumItem() const { return mNumItem; }
    void Load(const std::string& filename);
    float GetSimilarity(int iid1, int iid2) const {
        return (iid1 < iid2) ? mSimMat[iid2][iid1] : mSimMat[iid1][iid2];
    }
private:
    int mNumItem;
    std::vector<std::vector<float>> mSimMat; // triangle matrix
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H */
