/*
 * random_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_predict.h"
#include "common/common.h"

namespace longan {

RandomPredictOption::RandomPredictOption(const Json::Value& option) {
    mRatingRangeLow = option["ratingRangeLow"].asDouble();
    mRatingRangeHigh = option["ratingRangeHigh"].asDouble();
    assert(mRatingRangeLow < mRatingRangeHigh);
}

void RandomPredict::Init() {
    Log::I("recsys", "RandomPredict::Init()");
    LoadConfig();
    CreatePredictOption();
    LoadTrainData();
}

void RandomPredict::CreatePredictOption() {
    Log::I("recsys", "RandomPredict::CreatePredictOption()");
    mPredictOption = new RandomPredictOption(mConfig["predictOption"]);
}

float RandomPredict::PredictRating(int userId, int itemId) const {
    std::unique_lock<std::mutex> lock(mMutex);
    double predRating = Random::Instance().Uniform(
            mPredictOption->RatingRangeLow(), mPredictOption->RatingRangeHigh());
    return (float)predRating;
}

ItemIdList RandomPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(listSize > 0);
    int numItem = mTrainData->NumItem();
    std::vector<bool> itemsAvailable(numItem, true);
    const UserVec& uv = mTrainData->GetUserVector(userId);
    const ItemRating* data = uv.Data();
    int size = uv.Size();
    for (int i = 0; i < size; ++i) {
        itemsAvailable[data[i].ItemId()] = false;
    }
    ItemIdList topNItem(listSize);
    int count = 0;
    int limit = 0, maxLimit = 5000000;
    while ((++limit) < maxLimit) {
        int randItemid = -1;
        {
            std::unique_lock<std::mutex> lock(mMutex);
            randItemid = Random::Instance().Uniform(0, numItem);
        }
        if (itemsAvailable[randItemid]) {
            itemsAvailable[randItemid] = false;
            topNItem[count++] = randItemid;
            if (count == listSize) break;
        }
        if (count + size >= numItem) break;
    }
    return std::move(topNItem);
}

void RandomPredict::Cleanup() {
    delete mPredictOption;
    delete mTrainData;
}

} //~ namespace longan
