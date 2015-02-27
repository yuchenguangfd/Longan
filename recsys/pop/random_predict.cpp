/*
 * random_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_predict.h"
#include "common/math/math.h"
#include "common/util/random.h"
#include "common/logging/logging.h"

namespace longan {

RandomPredictOption::RandomPredictOption(const Json::Value& option) {
    mRatingRangeLow = option["ratingRangeLow"].asDouble();
    mRatingRangeHigh = option["ratingRangeHigh"].asDouble();
    mRoundIntRating = option["roundIntRating"].asBool();
}

void RandomPredict::Init() {
    Log::I("recsys", "RandomPredict::Init()");
    LoadConfig();
    CreateOption();
    LoadTrainData();
}

void RandomPredict::CreateOption() {
    Log::I("recsys", "RandomPredict::CreateOption()");
    mOption = new RandomPredictOption(mConfig["predictOption"]);
}

void RandomPredict::LoadTrainData() {
    Log::I("recsys", "RandomPredict::LoadTrainRating()");
    Log::I("recsys", "rating train file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
}

float RandomPredict::PredictRating(int userId, int itemId) const {
    std::unique_lock<std::mutex> lock(mMutex);
    double predRating = Random::Instance().Uniform(mOption->RatingRangeLow(), mOption->RatingRangeHigh());
    return mOption->RoundInt() ? Math::Round(predRating) : predRating;
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
    delete mOption;
    delete mTrainData;
}

} //~ namespace longan
