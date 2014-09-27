/*
 * pop_predict.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_predict.h"
#include "common/error.h"
#include <algorithm>
#include <cstdio>
#include <cassert>

namespace longan {

PopPredict::PopPredict(const std::string& ratingTrainFilePath, const std::string& modelFilePath) :
    mRatingTrainFilePath(ratingTrainFilePath),
    mModelFilePath(modelFilePath),
    mModelLoaded(false),
    mNumUser(0), mNumItem(0) { }

void PopPredict::Init() {
    FILE *fp = fopen(mModelFilePath.c_str(), "rb");
    assert(fp != nullptr);
    int rtn;
    rtn = fread((void*)&mNumItem, sizeof(mNumItem), 1, fp);
    assert(rtn == 1);
    mItemAverages.resize(mNumItem);
    rtn = fread((void*)&mItemAverages[0], sizeof(mItemAverages[0]), mNumItem, fp);
    assert(rtn == mNumItem);
    rtn = fread((void*)&mNumUser, sizeof(mNumUser), 1, fp);
    assert(rtn == 1);
    mUserAverages.resize(mNumUser);
    rtn = fread((void*)&mUserAverages[0], sizeof(mUserAverages[0]), mNumUser, fp);
    assert(rtn == mNumUser);
    fclose(fp);
    SortItemAverage();
    SortUserAverage();
    mModelLoaded = true;
}

void PopPredict::SortItemAverage() {
    mSortAverageItems.resize(mNumItem);
    for (int i = 0; i < mNumItem; ++i) {
        mSortAverageItems[i] = ItemRating(i, mItemAverages[i]);
    }
    std::sort(mSortAverageItems.begin(), mSortAverageItems.end(),
            [](const ItemRating& lhs, const ItemRating& rhs) {
                return lhs.Rating() > rhs.Rating();
    });
}

void PopPredict::SortUserAverage() {
    mSortAverageUsers.resize(mNumUser);
    for (int i = 0; i < mNumUser; ++i) {
        mSortAverageUsers[i] = UserRating(i, mUserAverages[i]);
    }
    std::sort(mSortAverageUsers.begin(), mSortAverageUsers.end(),
            [](const UserRating& lhs, const UserRating& rhs) {
                return lhs.Rating() > rhs.Rating();
    });
}

float PopPredict::PredictRating(int userId, int itemId) {
    assert(mModelLoaded);
    assert(itemId >= 0 && itemId < mNumItem);
    float predRating = mItemAverages[itemId];
    return predRating;
}

RecommendedItemIdList PopPredict::PredictItemTopN(int userId, int listSize) {
    assert(mModelLoaded);
    // TODO
    RecommendedItemIdList items(listSize);
    for (int i = 0; i < listSize; ++i) {
        items[i] = mSortAverageItems[i].ItemId();
    }
    return items;
}

RecommendedUserIdList PopPredict::PredictUserTopN(int itemId, int listSize) {
    assert(mModelLoaded);
    // TODO
    RecommendedUserIdList users(listSize);
    for (int i = 0; i < listSize; ++i) {
        users[i] = mSortAverageUsers[i].UserId();
    }
    return users;
}

} //~ namespace longan
