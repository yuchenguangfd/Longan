/*
 * user_based_model.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_MODEL_H
#define RECSYS_NEIGHBOR_USER_BASED_MODEL_H

#include "user_based_util.h"
#include "common/util/running_statistic.h"
#include <vector>
#include <string>

namespace longan {

namespace UserBased {

class NeighborUser {
public:
    NeighborUser(int uid, float sim, float rating) : mUserId(uid), mSimilarity(sim),
        mRating(rating) { }
    int UserId() const { return mUserId; }
    float Similarity() const { return mSimilarity; }
    float Rating() const { return mRating; }
private:
    int mUserId;
    float mSimilarity;
    float mRating;
};

inline bool operator < (const NeighborUser& lhs, const NeighborUser& rhs) {
    return lhs.Similarity() < rhs.Similarity();
}

class ModelTrain {
public:
    ModelTrain(const Parameter *param, int numUser);
    int NumUser() const { return mNumUser; }
    const Parameter* GetParameter() const { return mParameter; }
    void Save(const std::string& filename);
    void PutSimilarity(int uid1, int uid2, float sim) {
        if (uid1 < uid2) {
            mSimMat[uid2][uid1] = sim;
        } else {
            mSimMat[uid1][uid2] = sim;
        }
    }
    float GetSimilarity(int uid1, int uid2) const {
        return (uid1 < uid2) ? mSimMat[uid2][uid1] : mSimMat[uid1][uid2];
    }
private:
    const Parameter *mParameter;
    int mNumUser;
    std::vector<std::vector<float>> mSimMat; // triangle matrix
};

class ModelPredict {
public:
    ModelPredict();
    int NumUser() const { return mNumUser; }
    void Load(const std::string& filename);
    float GetSimilarity(int uid1, int uid2) const {
        return (uid1 < uid2) ? mSimMat[uid2][uid1] : mSimMat[uid1][uid2];
    }
private:
    int mNumUser;
    std::vector<std::vector<float>> mSimMat; // triangle matrix
};

} //~ namespace UserBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_MODEL_H */
