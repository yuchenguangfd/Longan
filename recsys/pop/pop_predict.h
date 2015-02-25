/*
 * pop_predict.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_PREDICT_H
#define RECSYS_POP_POP_PREDICT_H

#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include <vector>

namespace longan {

class PopPredictOption {
public:
    PopPredictOption(const Json::Value& option);
    bool PredictRatingByItemAverage() const { return mPredictRatingByItemAverage; }
    bool PredictRatingByUserAverage() const { return mPredictRatingByUserAverage; }
    bool PredictRankingByItemAverage() const { return mPredictRankingByItemAverage; }
    bool PredictRankingByItemPopularity() const { return mPredictRankingByItemPopularity; }
    bool RoundInt() const { return mRoundIntRating; }
private:
    bool mPredictRatingByItemAverage;
    bool mPredictRatingByUserAverage;
    bool mPredictRankingByItemAverage;
    bool mPredictRankingByItemPopularity;
    bool mRoundIntRating;
};

class PopPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void CreateOption();
    void LoadTrainData();
    void LoadModel();
    void SortItemRatings();
private:
    const PopPredictOption *mOption = nullptr;
    RatingMatUsers *mTrainData = nullptr;
    std::vector<float> mUserAverages;
    std::vector<float> mItemAverages;
    std::vector<float> mItemPopularities;
    std::vector<ItemRating> mSortedItemRatings;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_PREDICT_H */
