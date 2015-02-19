/*
 * pop_predict.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_predict.h"
#include "common/logging/logging.h"
#include "common/lang/binary_input_stream.h"
#include "common/base/algorithm.h"
#include <algorithm>
#include <cassert>

namespace longan {

void PopPredict::Init() {
    Log::I("recsys", "PopPredict::Init()");
    LoadTrainRating();
    LoadModel();
    SortItemsByAverage();
}

void PopPredict::Cleanup() { }

void PopPredict::LoadTrainRating() {
    Log::I("recsys", "PopPredict::LoadTrainRating()");
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainRating.Init(rlist);
}

void PopPredict::LoadModel() {
    Log::I("recsys", "PopPredict::LoadModel()");
    BinaryInputStream bis(mModelFilepath);
    int numItem;
    bis >> numItem;
    mItemAverages.resize(numItem);
    for (int i = 0; i < numItem; ++i) {
        bis >> mItemAverages[i];
    }
    assert(mItemAverages.size() == mTrainRating.NumItem());
}

void PopPredict::SortItemsByAverage() {
    Log::I("recsys", "PopPredict::SortItemsByAverage()");
    mSortedItemAverages.resize(mTrainRating.NumItem());
    for (int i = 0; i < mTrainRating.NumItem(); ++i) {
        mSortedItemAverages[i] = ItemRating(i, mItemAverages[i]);
    }
    std::sort(mSortedItemAverages.begin(), mSortedItemAverages.end(),
        [](const ItemRating& lhs, const ItemRating& rhs)->bool {
            return (lhs.Rating() > rhs.Rating());
    });
}

float PopPredict::PredictRating(int userId, int itemId) const {
    assert(itemId >= 0 && itemId < mTrainRating.NumItem());
    float predRating = mItemAverages[itemId];
    return predRating;
}

ItemIdList PopPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mTrainRating.NumUser());
    assert(listSize > 0);
    ItemIdList topItems;
    topItems.reserve(listSize);
    const UserVec& uv = mTrainRating.GetUserVector(userId);
    for (int i = 0; i < mSortedItemAverages.size(); ++i) {
        int iid = mSortedItemAverages[i].ItemId();
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

} //~ namespace longan
