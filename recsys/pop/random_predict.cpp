/*
 * random_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_predict.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"
#include "common/util/array_helper.h"
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
    std::vector<int> randItems(mTrainData->NumItem());
    ArrayHelper::FillRange(randItems.data(), randItems.size());
    {
        std::unique_lock<std::mutex> lock(mMutex);
        ArrayHelper::RandomShuffle(randItems.data(), randItems.size());
    }
    ItemIdList topNItem(listSize);
    int count = 0;
    for (int i = 0; i < randItems.size(); ++i) {
        int iid = randItems[i];
        const UserVec& uv = mTrainData->GetUserVector(userId);
        int pos = BSearch(iid, uv.Data(), uv.Size(),
            [](int lhs, const ItemRating& rhs)->int{
                return lhs - rhs.ItemId();
        });
        if (pos == -1) {
            topNItem[count++] = iid;
            if (count == listSize) break;
        }
    }
    return std::move(topNItem);
}

void RandomPredict::Cleanup() {
    delete mOption;
    delete mTrainData;
}

} //~ namespace longan
