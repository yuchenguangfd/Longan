/*
 * pop_predict.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_predict.h"
#include "common/common.h"

namespace longan {

namespace Pop {

PredictOption::PredictOption(const Json::Value& option) {
    if (option["predictRatingMethod"].asString() == "userAverage") {
        mPredictRatingMethod = PredictRatingMethod_UserAverage;
    } else if (option["predictRatingMethod"].asString() == "itemAverage") {
        mPredictRatingMethod = PredictRatingMethod_ItemAverage;
    } else {
        throw LonganConfigError();
    }
    if (option["predictRankingMethod"].asString() == "itemPopularity") {
        mPredictRankingMethod = PredictRankingMethod_ItemPopularity;
    } else if (option["predictRankingMethod"].asString() == "itemAverage") {
        mPredictRankingMethod = PredictRankingMethod_ItemAverage;
    } else {
        throw LonganConfigError();
    }
}

} //~ namespace Pop

void PopPredict::CreatePredictOption() {
    Log::I("recsys", "PopPredict::CreatePredictOption()");
    mPredictOption = new Pop::PredictOption(mConfig["predictOption"]);
}

void PopPredict::LoadModel() {
    Log::I("recsys", "PopPredict::LoadModel()");
    BinaryInputStream bis(mModelFilepath);
    int numItem;
    bis >> numItem;
    assert(numItem == mTrainData->NumItem());
    mItemAverages.resize(numItem);
    bis.Read(mItemAverages.data(), mItemAverages.size());
    mItemPopularities.resize(numItem);
    bis.Read(mItemPopularities.data(), mItemPopularities.size());
    int numUser;
    bis >> numUser;
    assert(numUser == mTrainData->NumUser());
    mUserAverages.resize(numUser);
    bis.Read(mUserAverages.data(), mUserAverages.size());
}

float PopPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(itemId >= 0 && itemId < mTrainData->NumItem());
    float predRating = 0.0f;
    if (mPredictOption->PredictRatingMethod() == Pop::PredictOption::PredictRatingMethod_ItemAverage) {
        predRating = mItemAverages[itemId];
    } else if (mPredictOption->PredictRatingMethod() == Pop::PredictOption::PredictRatingMethod_UserAverage) {
        predRating = mUserAverages[userId];
    }
    return predRating;
}

float PopPredict::ComputeTopNItemScore(int userId, int itemId) const {
    float score = 0.0f;
    if (mPredictOption->PredictRankingMethod() == Pop::PredictOption::PredictRankingMethod_ItemAverage) {
        score = mItemAverages[itemId];
    } else if (mPredictOption->PredictRankingMethod() == Pop::PredictOption::PredictRankingMethod_ItemPopularity) {
        score = mItemPopularities[itemId];
    }
    return score;
}

void PopPredict::Cleanup() {
    Log::I("recsys", "PopPredict::Cleanup()");
    delete mPredictOption;
    delete mTrainData;
}

} //~ namespace longan
