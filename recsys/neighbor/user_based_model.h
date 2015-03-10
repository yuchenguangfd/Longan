/*
 * user_based_model.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_MODEL_H
#define RECSYS_NEIGHBOR_USER_BASED_MODEL_H

#include "user_based_util.h"
#include "recsys/base/similarity_matrix.h"
#include <vector>
#include <string>

namespace longan {

namespace UserBased {

class NeighborUser {
public:
    NeighborUser(float sim, float rating) : mSimilarity(sim), mRating(rating) { }
    float Similarity() const { return mSimilarity; }
    float Rating() const { return mRating; }
private:
    float mSimilarity;
    float mRating;
};

inline bool operator< (const NeighborUser& lhs, const NeighborUser& rhs) {
    return lhs.Similarity() < rhs.Similarity();
}

class Model : public SimilarityMatrix {
public:
    Model(const Parameter *param, int numUser);
    int NumUser() const { return mSize; }
    const Parameter* GetParameter() const { return mParameter; }
    void Save(const std::string& filename);
    void Load(const std::string& filename);
private:
    const Parameter *mParameter;
};

} //~ namespace UserBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_MODEL_H */
