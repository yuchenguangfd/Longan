/*
 * svd_model.h
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_MODEL_H
#define RECSYS_SVD_SVD_MODEL_H

#include "svd_util.h"
#include "common/math/vector.h"
#include <string>
#include <vector>
#include <cassert>

namespace longan {

class SVDTrain;

namespace SVD {

class ModelComputation;
class ModelComputationST;
class ModelComputationMT;

class Model {
public:
    Model(const Parameter *param);
    int NumUser() const { return mNumUser; }
    int NumItem() const { return mNumItem; }
    float RatingAverage() const { return mRatingAverage; }
    const Vector32F& UserFeature(int userId) const { return mUserFeatures[userId]; }
    const Vector32F& ItemFeature(int itemId) const { return mItemFeatures[itemId]; }
    float UserBias(int userId) const { return mUserBiases[userId]; }
    float ItemBias(int itemId) const { return mItemBiases[itemId]; }
    void Load(const std::string& filename);
    void Save(const std::string& filename);
protected:
    const Parameter *mParameter;
    int mNumUser;
    int mNumItem;
    float mRatingAverage;
    std::vector<Vector32F> mUserFeatures;
    std::vector<Vector32F> mItemFeatures;
    std::vector<float> mUserBiases;
    std::vector<float> mItemBiases;
};

class ModelTrain : public Model {
public:
    ModelTrain(const Parameter *param, int numUser, int numItem, float ratingAverage);
    void RandomInit();
    friend ModelComputation;
    friend ModelComputationST;
    friend ModelComputationMT;
    friend SVDTrain;
};

} //~ namespace SVD

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_MODEL_H */
