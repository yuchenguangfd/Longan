/*
 * rating_trait.cpp
 * Created on: Oct 5, 2014
 * Author: chenguangyu
 */

#include "rating_trait.h"
#include "common/util/running_statistic.h"

namespace longan {

RatingTrait::RatingTrait() :
    mNumRating(0),
    mNumUser(0),
    mNumItem(0),
    mMin(0.0f),
    mMax(0.0f),
    mAverage(0.0f),
    mStd(0.0f) { }

void RatingTrait::Init(const RatingList& rlist) {
    mNumRating = rlist.Size();
    mNumUser = rlist.NumUser();
    mNumItem = rlist.NumItem();
    RunningMin<float> runningAllMin;
    RunningMax<float> runningAllMax;
    RunningStd<float> runningAllStd(0.0f);
    std::vector<RunningMin<float>> userRunningMins(mNumUser);
    std::vector<RunningMax<float>> userRunningMaxs(mNumUser);
    std::vector<RunningMin<float>> itemRunningMins(mNumItem);
    std::vector<RunningMax<float>> itemRunningMaxs(mNumItem);
    std::vector<RunningStd<float>> userRunningStds(mNumUser);
    std::vector<RunningStd<float>> itemRunningStds(mNumItem);
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
    mMin = runningAllMin.CurrentMin();
    mMax = runningAllMax.CurrentMax();
    mAverage = runningAllStd.CurrentMean();
    mStd = runningAllStd.CurrentStd();
    mUserMins.resize(mNumUser);
    mUserMaxs.resize(mNumUser);
    mUserActivities.resize(mNumUser);
    mUserAverages.resize(mNumUser);
    mUserStds.resize(mNumUser);
    for (int uid = 0; uid < mNumUser; ++uid) {
        mUserMins[uid] = userRunningMins[uid].CurrentMin();
        mUserMaxs[uid] = userRunningMaxs[uid].CurrentMax();
        mUserActivities[uid] = userRunningStds[uid].CurrentCount();
        mUserAverages[uid] = userRunningStds[uid].CurrentMean();
        mUserStds[uid] = userRunningStds[uid].CurrentStd();
    }
    mItemMins.resize(mNumItem);
    mItemMaxs.resize(mNumItem);
    mItemPopularities.resize(mNumItem);
    mItemAverages.resize(mNumItem);
    mItemStds.resize(mNumItem);
    for (int iid = 0; iid < mNumItem; ++iid) {
        mItemMins[iid] = itemRunningMins[iid].CurrentMin();
        mItemMaxs[iid] = itemRunningMaxs[iid].CurrentMax();
        mItemPopularities[iid] = itemRunningStds[iid].CurrentCount();
        mItemAverages[iid] = itemRunningStds[iid].CurrentMean();
        mItemStds[iid] = itemRunningStds[iid].CurrentStd();
    }
}

} //~ namespace longan
