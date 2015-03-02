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
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
    virtual float ComputeItemSimilarity(int itemId1, int itemId2) const override;
private:
    void CreatePredictOption();
    void CreateParameter();
    void LoadTrainData();
    void AdjustRating();
    void LoadModel();
    void InitCachedTopNItems();
    float PredictRatingAllNeighbor(int userId, int itemId) const;
    float PredictRatingFixedSizeNeighbor(int userId, int itemid) const;
    ItemIdList PredictTopNItemFromCache(int userId, int listSize) const;
private:
    const UserBased::PredictOption *mPredictOption = nullptr;
    const UserBased::Parameter *mParameter = nullptr;
    RatingMatItems *mTrainDataItems = nullptr;
    RatingMatUsers *mTrainDataUsers = nullptr;
    RatingTrait *mTrainDataTrait = nullptr;
    UserBased::ModelPredict *mModel = nullptr;
    mutable std::vector<ItemIdList> mCachedTopNItems;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_PREDICT_H */
