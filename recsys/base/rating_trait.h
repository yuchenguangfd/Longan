/*
 * rating_trait.h
 * Created on: Oct 5, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_TRAIT_H
#define RECSYS_BASE_RATING_TRAIT_H

#include "rating_list.h"
#include <vector>
#include <cassert>

namespace longan {

class RatingTrait {
public:
    RatingTrait();
    void Init(const RatingList& rlist);
    float Min() const { return mMin; }
    float Max() const { return mMax; }
    float UserMin(int userId) const { return mUserMins[userId]; }
    float UserMax(int userId) const { return mUserMaxs[userId]; }
    float ItemMin(int itemId) const { return mItemMins[itemId]; }
    float ItemMax(int itemId) const { return mItemMaxs[itemId]; }
    float Average() const { return mAverage; }
    float Std() const { return mStd; }
    int UserActivity(int userId) const { return mUserActivities[userId]; }
    float UserAverage(int userId) const { return mUserAverages[userId]; }
    float UserStd(int userId) const { return mUserStds[userId]; }
    int ItemPopularity(int itemId) const { return mItemPopularities[itemId]; }
    float ItemAverage(int itemId) const { return mItemAverages[itemId]; }
    float ItemStd(int itemId) const { return mItemStds[itemId]; }
private:
    int mNumRating;
    int mNumUser;
    int mNumItem;

    float mMin;
    float mMax;
    std::vector<float> mUserMins;
    std::vector<float> mUserMaxs;
    std::vector<float> mItemMins;
    std::vector<float> mItemMaxs;

    float mAverage;
    float mStd;
    std::vector<int> mUserActivities;
    std::vector<float> mUserAverages;
    std::vector<float> mUserStds;
    std::vector<int> mItemPopularities;
    std::vector<float> mItemAverages;
    std::vector<float> mItemStds;
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_TRAIT_H */
