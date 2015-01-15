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
class SimpleModelComputation;
class FPSGDModelComputation;

class Model {
public:
    Model(const Parameter& param);
    ~Model();
    int NumUser() const { return mNumUser; }
    int NumItem() const { return mNumItem; }
    float RatingAverage() const { return mRatingAverage; }
    const Vector<float>& UserFeature(int userId) const { return mUserFeatures[userId]; }
    const Vector<float>& ItemFeature(int itemId) const { return mItemFeatures[itemId]; }
    float UserBias(int userId) const { return mUserBiases[userId]; }
    float ItemBias(int itemId) const { return mItemBiases[itemId]; }
protected:
    const Parameter& mParameter;
    int mNumUser;
    int mNumItem;
    float mRatingAverage;
    std::vector<Vector<float>> mUserFeatures;
    std::vector<Vector<float>> mItemFeatures;
    std::vector<float> mUserBiases;
    std::vector<float> mItemBiases;
};

class ModelTrain : public Model {
public:
    ModelTrain(const Parameter& param, int numUser, int numItem, float ratingAvg);
    void RandomInit();
    void Save(const std::string& filename);
    friend ModelComputation;
    friend SimpleModelComputation;
    friend FPSGDModelComputation;
    friend SVDTrain;
};

class ModelPredict : public Model {
public:
    using Model::Model;
    void Load(const std::string& filename);
};

} //~ namespace SVD

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_MODEL_H */
