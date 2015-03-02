/*
 * item_based_predict.h
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_ITEM_BASED_PREDICT_H

#include "item_based_util.h"
#include "item_based_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_trait.h"

namespace longan {

class ItemBasedPredict : public BasicPredict {
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
    float PredictRatingFixedSizeNeighbor(int userId, int itemId) const;
    ItemIdList PredictTopNItemFromCache(int userId, int listSize) const;
private:
    const ItemBased::PredictOption *mPredictOption = nullptr;
    const ItemBased::Parameter *mParameter = nullptr;
    RatingMatUsers *mTrainData = nullptr;
    RatingTrait *mTrainDataTrait = nullptr;
    ItemBased::ModelPredict *mModel = nullptr;
    mutable std::vector<ItemIdList> mCachedTopNItems;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
