/*
 * random_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_predict.h"
#include "common/base/algorithm.h"
#include "common/util/running_statistic.h"
#include "common/util/random.h"
#include "common/util/array_helper.h"
#include "common/logging/logging.h"

namespace longan {

void RandomPredict::Init() {
    Log::I("recsys", "RandomPredict::Init()");
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    std::vector<RunningMax<float>> runUserMaxRatings(rlist.NumUser());
    std::vector<RunningMin<float>> runUserMinRatings(rlist.NumUser());
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        runUserMaxRatings[rr.UserId()].Add(rr.Rating());
        runUserMinRatings[rr.UserId()].Add(rr.Rating());
    }
    mUserMaxRatings.resize(rlist.NumUser());
    mUserMinRatings.resize(rlist.NumUser());
    for (int i = 0; i < rlist.NumUser(); ++i) {
        mUserMaxRatings[i] = runUserMaxRatings[i].CurrentMax();
        mUserMinRatings[i] = runUserMinRatings[i].CurrentMin();
    }

    mRatingMatrix.Init(rlist);

    mRandomItemIds.resize(rlist.NumItem());
    ArrayHelper::FillRange(&mRandomItemIds[0], mRandomItemIds.size());
}

void RandomPredict::Cleanup() { }

float RandomPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mRatingMatrix.NumUser());
    double lower = mUserMinRatings[userId];
    double upper = mUserMaxRatings[userId];
    double predictedRating = Random::Instance().Uniform(lower, upper);
    return (float)predictedRating;
}

ItemIdList RandomPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mRatingMatrix.NumUser());
    assert(listSize > 0 && listSize < mRatingMatrix.NumItem());
    ArrayHelper::RandomShuffle(&mRandomItemIds[0], mRandomItemIds.size());
    ItemIdList topNItem;
    topNItem.reserve(listSize);
    for (int i = 0; i < mRandomItemIds.size(); ++i) {
        int iid = mRandomItemIds[i];
        const auto& uv = mRatingMatrix.GetUserVector(userId);
        int pos = BSearch(iid, uv.Data(), uv.Size(),
            [](int lhs, const ItemRating& rhs)->int{
                return lhs - rhs.ItemId();
        });
        if (pos == -1) {
            topNItem.push_back(iid);
            if (topNItem.size() == listSize) break;
        }
    }
    if (topNItem.size() < listSize) {
        for (int i = topNItem.size(); i < listSize; ++i) {
            topNItem.push_back(-1);
        }
    }
    return std::move(topNItem);
}

} //~ namespace longan
