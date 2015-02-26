/*
 * random_predict.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_RANDOM_PREDICT_H
#define RECSYS_POP_RANDOM_PREDICT_H

#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include <mutex>

namespace longan {

class RandomPredictOption {
public:
    RandomPredictOption(const Json::Value& option);
    double RatingRangeLow() const { return mRatingRangeLow; }
    double RatingRangeHigh() const { return mRatingRangeHigh; }
    bool RoundInt() const { return mRoundIntRating; }
private:
    double mRatingRangeLow;
    double mRatingRangeHigh;
    bool mRoundIntRating;
};

class RandomPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void CreateOption();
    void LoadTrainData();
private:
    const RandomPredictOption *mOption = nullptr;
    RatingMatUsers *mTrainData = nullptr;
    mutable std::mutex mMutex;
};

} //~ namespace longan

#endif /* RECSYS_POP_RANDOM_PREDICT_H */
