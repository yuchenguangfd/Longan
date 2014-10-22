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
    float MinRating() const {
        return mMinRating;
    }
    float MaxRating() const {
        return mMaxRating;
    }
    float UserMinRating(int userId) const {
        assert(userId >= 0 && userId < mUserMinRatings.size());
        return mUserMinRatings[userId];
    }
    float UserMaxRating(int userId) const {
        assert(userId >= 0 && userId < mUserMaxRatings.size());
        return mUserMaxRatings[userId];
    }
    float ItemMinRating(int itemId) const {
        assert(itemId >= 0 && itemId < mItemMinRatings.size());
        return mItemMinRatings[itemId];
    }
    float ItemMaxRating(int itemId) const {
        assert(itemId >= 0 && itemId < mItemMaxRatings.size());
        return mItemMaxRatings[itemId];
    }
    float AverageRating() const {
        return mAverageRating;
    }
    float StdRating() const {
        return mStdRating;
    }
    float UserAverageRating(int userId) const {
        assert(userId >= 0 && userId < mUserAverageRatings.size());
        return mUserAverageRatings[userId];
    }
    float UserStdRating(int userId) const {
        assert(userId >= 0 && userId < mUserStdRatings.size());
        return mUserStdRatings[userId];
    }
    float ItemAverageRating(int itemId) const {
        assert(itemId >= 0 && itemId < mItemAverageRatings.size());
        return mItemAverageRatings[itemId];
    }
    float ItemStdRating(int itemId) const {
        assert(itemId >= 0 && itemId < mItemStdRatings.size());
        return mItemStdRatings[itemId];
    }
private:
    int mNumRating;
    int mNumUser;
    int mNumItem;

    float mMinRating;
    float mMaxRating;
    std::vector<float> mUserMinRatings;
    std::vector<float> mUserMaxRatings;
    std::vector<float> mItemMinRatings;
    std::vector<float> mItemMaxRatings;

    float mAverageRating;
    float mStdRating;
    std::vector<float> mUserAverageRatings;
    std::vector<float> mItemAverageRatings;
    std::vector<float> mUserStdRatings;
    std::vector<float> mItemStdRatings;
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_TRAIT_H */
