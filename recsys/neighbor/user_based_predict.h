/*
 * user_based_predict.h
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_USER_BASED_PREDICT_H

#include "user_based_util.h"
#include "user_based_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_trait.h"

namespace longan {

class UserBasedPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
protected:
    virtual void CreatePredictOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void LoadModel() override;
    virtual float ComputeTopNItemScore(int userId, int itemId) const override;
private:
    float PredictRatingByAllNeighbor(int userId, int itemId) const;
    float PredictRatingByFixedSizeNeighbor(int userId, int itemId) const;
    float ComputeTopNItemScoreByAllNeighbor(int userId, int itemId) const;
    float ComputeTopNItemScoreByFixedSizeNeighbor(int userId, int itemId) const;
private:
    const UserBased::PredictOption *mPredictOption = nullptr;
    const UserBased::Parameter *mParameter = nullptr;
    RatingTrait *mTrainDataTrait = nullptr;
    UserBased::Model *mModel = nullptr;
    RatingMatItems *mTrainDataItems = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_PREDICT_H */
