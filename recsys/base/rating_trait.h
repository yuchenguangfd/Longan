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
    float UserMin(int userId) const {
        assert(userId >= 0 && userId < mUserMins.size());
        return mUserMins[userId];
    }
    float UserMax(int userId) const {
        assert(userId >= 0 && userId < mUserMaxs.size());
        return mUserMaxs[userId];
    }
    float ItemMin(int itemId) const {
        assert(itemId >= 0 && itemId < mItemMins.size());
        return mItemMins[itemId];
    }
    float ItemMax(int itemId) const {
        assert(itemId >= 0 && itemId < mItemMaxs.size());
        return mItemMaxs[itemId];
    }
    float Average() const {
        return mAverage;
    }
    float Std() const {
        return mStd;
    }
    float UserAverage(int userId) const {
        assert(userId >= 0 && userId < mUserAverages.size());
        return mUserAverages[userId];
    }
    float UserStd(int userId) const {
        assert(userId >= 0 && userId < mUserStds.size());
        return mUserStds[userId];
    }
    float ItemAverage(int itemId) const {
        assert(itemId >= 0 && itemId < mItemAverages.size());
        return mItemAverages[itemId];
    }
    float ItemStd(int itemId) const {
        assert(itemId >= 0 && itemId < mItemStds.size());
        return mItemStds[itemId];
    }
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
    std::vector<float> mUserAverages;
    std::vector<float> mItemAverages;
    std::vector<float> mUserStds;
    std::vector<float> mItemStds;
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_TRAIT_H */
