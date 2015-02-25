/*
 * pop_predict.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_predict.h"
#include "common/math/math.h"
#include "common/logging/logging.h"
#include "common/lang/binary_input_stream.h"
#include "common/base/algorithm.h"
#include <algorithm>
#include <cassert>

namespace longan {

PopPredictOption::PopPredictOption(const Json::Value& option) {
    if (option["predictRatingBy"].asString() == "userAverage") {
        mPredictRatingByUserAverage = true;
        mPredictRatingByItemAverage = false;
    } else if (option["predictRatingBy"].asString() == "itemAverage") {
        mPredictRatingByUserAverage = false;
        mPredictRatingByItemAverage = true;
    } else {
        throw LonganConfigError();
    }
    if (option["predictRankingBy"].asString() == "itemPopularity") {
        mPredictRankingByItemAverage = false;
        mPredictRankingByItemPopularity = true;
    } else if (option["predictRankingBy"].asString() == "itemAverage") {
        mPredictRankingByItemAverage = true;
        mPredictRankingByItemPopularity = false;
    } else {
        throw LonganConfigError();
    }
    mRoundIntRating = option["roundIntRating"].asBool();
}

void PopPredict::Init() {
    Log::I("recsys", "PopPredict::Init()");
    LoadConfig();
    CreateOption();
    LoadTrainData();
    LoadModel();
    SortItemRatings();
}

void PopPredict::CreateOption() {
    Log::I("recsys", "PopPredict::CreateOption()");
    mOption = new PopPredictOption(mConfig["predictOption"]);
}

void PopPredict::LoadTrainData() {
    Log::I("recsys", "PopPredict::LoadTrainRating()");
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
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

void PopPredict::SortItemRatings() {
    Log::I("recsys", "PopPredict::SortItemRatings()");
    mSortedItemRatings.resize(mTrainData->NumItem());
    for (int i = 0; i < mTrainData->NumItem(); ++i) {
        if (mOption->PredictRankingByItemAverage()) {
            mSortedItemRatings[i] = ItemRating(i, mItemAverages[i]);
        } else if (mOption->PredictRankingByItemPopularity()) {
            mSortedItemRatings[i] = ItemRating(i, mItemPopularities[i]);
        }
    }
    std::sort(mSortedItemRatings.begin(), mSortedItemRatings.end(),
        [](const ItemRating& lhs, const ItemRating& rhs)->bool {
            return (lhs.Rating() > rhs.Rating());
    });
}

float PopPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(itemId >= 0 && itemId < mTrainData->NumItem());
    float predRating = 0.0f;
    if (mOption->PredictRatingByItemAverage()) {
        predRating = mItemAverages[itemId];
    } else if (mOption->PredictRatingByUserAverage()) {
        predRating = mUserAverages[userId];
    }
    return mOption->RoundInt() ? Math::Round(predRating) : predRating;
}

ItemIdList PopPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(listSize > 0);
    ItemIdList topItems;
    topItems.reserve(listSize);
    const UserVec& uv = mTrainData->GetUserVector(userId);
    for (int i = 0; i < mSortedItemRatings.size(); ++i) {
        int iid = mSortedItemRatings[i].ItemId();
        int pos = BSearch(iid, uv.Data(), uv.Size(),
            [](int lhs, const ItemRating& rhs)->int {
                return lhs - rhs.ItemId();
            });
        if (pos == -1) {
            topItems.push_back(iid);
            if (topItems.size() == listSize) break;
        }
    }
    if (topItems.size() < listSize) {
        for (int i = topItems.size(); i < listSize; ++i) {
            topItems.push_back(-1);
        }
    }
    return std::move(topItems);
}

void PopPredict::Cleanup() {
    delete mOption;
    delete mTrainData;
}

} //~ namespace longan
