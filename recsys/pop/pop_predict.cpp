/*
 * pop_predict.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_predict.h"
#include "recsys/base/rating_list_loader.h"
#include "common/logging/logging.h"
#include "common/lang/binary_input_stream.h"
#include "common/base/algorithm.h"
#include "algorithm/sort/quick_sort.h"
#include <cassert>

namespace longan {

void PopPredict::Init() {
    Log::I("recsys", "PopPredict::Init()");
    LoadRatings();
    LoadModel();
    assert(mItemAverages.size() == mRatingMatrix.NumItem());
    SortItemAverages();
}

void PopPredict::Cleanup() { }

void PopPredict::LoadRatings() {
    RatingList rlist = RatingListLoader::Load(mRatingTrainFilepath);
    mRatingMatrix.Init(rlist);
}

void PopPredict::LoadModel() {
    BinaryInputStream bis(mModelFilepath);
    int numItem;
    bis >> numItem;
    mItemAverages.resize(numItem);
    for (int i = 0; i < numItem; ++i) {
        bis >> mItemAverages[i];
    }
}

void PopPredict::SortItemAverages() {
    mSortedItemAverages.resize(mRatingMatrix.NumItem());
    for (int i = 0; i < mRatingMatrix.NumItem(); ++i) {
        mSortedItemAverages[i] = ItemRating(i, mItemAverages[i]);
    }
    QuickSort sort;
    sort(&mSortedItemAverages[0], mSortedItemAverages.size(),
            [](const ItemRating& lhs, const ItemRating& rhs)->int {
                return (lhs.Rating() > rhs.Rating()) ? -1: 1;
    });
}

float PopPredict::PredictRating(int userId, int itemId) const {
    assert(itemId >= 0 && itemId < mRatingMatrix.NumItem());
    float predictedRating = mItemAverages[itemId];
    return predictedRating;
}

ItemIdList PopPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mRatingMatrix.NumUser());
    assert(listSize > 0 && listSize < mRatingMatrix.NumItem());
    ItemIdList topNItem;
    topNItem.reserve(listSize);
    const auto& uv = mRatingMatrix.GetUserVector(userId);
    for (int i = 0; i < mSortedItemAverages.size(); ++i) {
        int iid = mSortedItemAverages[i].ItemId();
        int pos = BSearch(iid, uv.Data(), uv.Size(),
            [](int lhs, const ItemRating& rhs)->int {
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
