/*
 * rating_trait.cpp
 * Created on: Oct 5, 2014
 * Author: chenguangyu
 */

#include "rating_trait.h"
#include "common/math/running_statistic.h"

namespace longan {

RatingTrait::RatingTrait() :
    mNumRating(0),
    mNumUser(0),
    mNumItem(0),
    mMinRating(0.0f),
    mMaxRating(0.0f),
    mAverageRating(0.0f),
    mStdRating(0.0f) { }

void RatingTrait::Init(const RatingList& rlist) {
    mNumRating = rlist.Size();
    mNumUser = rlist.NumUser();
    mNumItem = rlist.NumItem();
    RunningMin<float> runningAllMin;
    RunningMax<float> runningAllMax;
    RunningStd<float> runningAllStd(0.0f);
    std::vector<RunningMin<float> > userRunningMins(mNumUser);
    std::vector<RunningMax<float> > userRunningMaxs(mNumUser);
    std::vector<RunningMin<float> > itemRunningMins(mNumItem);
    std::vector<RunningMax<float> > itemRunningMaxs(mNumItem);
    std::vector<RunningStd<float> > userRunningStds(mNumUser);
    std::vector<RunningStd<float> > itemRunningStds(mNumItem);
    for (int i = 0; i < rlist.Size(); ++i) {
        const RatingRecord& record = rlist[i];
        runningAllMin.Add(record.Rating());
        runningAllMax.Add(record.Rating());
        userRunningMins[record.UserId()].Add(record.Rating());
        userRunningMaxs[record.UserId()].Add(record.Rating());
        itemRunningMins[record.ItemId()].Add(record.Rating());
        itemRunningMaxs[record.ItemId()].Add(record.Rating());
        runningAllStd.Add(record.Rating());
        userRunningStds[record.UserId()].Add(record.Rating());
        itemRunningStds[record.ItemId()].Add(record.Rating());
    }
    mMinRating = runningAllMin.CurrentMin();
    mMaxRating = runningAllMax.CurrentMax();
    mAverageRating = runningAllStd.CurrentMean();
    mStdRating = runningAllStd.CurrentStd();
    mUserMinRatings.resize(mNumUser);
    mUserMaxRatings.resize(mNumUser);
    mUserAverageRatings.resize(mNumUser);
    mUserStdRatings.resize(mNumUser);
    for (int uid = 0; uid < mNumUser; ++uid) {
        mUserMinRatings[uid] = userRunningMins[uid].CurrentMin();
        mUserMaxRatings[uid] = userRunningMaxs[uid].CurrentMax();
        mUserAverageRatings[uid] = userRunningStds[uid].CurrentMean();
        mUserStdRatings[uid] = userRunningStds[uid].CurrentStd();
    }
    mItemMinRatings.resize(mNumItem);
    mItemMaxRatings.resize(mNumItem);
    mItemAverageRatings.resize(mNumItem);
    mItemStdRatings.resize(mNumItem);
    for (int iid = 0; iid < mNumItem; ++iid) {
        mItemMinRatings[iid] = itemRunningMins[iid].CurrentMin();
        mItemMaxRatings[iid] = itemRunningMaxs[iid].CurrentMax();
        mItemAverageRatings[iid] = itemRunningStds[iid].CurrentMean();
        mItemStdRatings[iid] = itemRunningStds[iid].CurrentStd();
    }
}

} //~ namespace longan
