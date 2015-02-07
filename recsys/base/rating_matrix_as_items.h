/*
 * rating_matrix_as_items.h
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_MATRIX_AS_ITEMS_H
#define RECSYS_BASE_RATING_MATRIX_AS_ITEMS_H

#include "rating_list.h"
#include "item_vector.h"
#include "algorithm/sort/quick_sort_mt.h"
#include "common/lang/defines.h"
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace longan {

template <class ItemVectorAlloc = std::allocator<UserRating>>
class RatingMatrixAsItems {
public:
    RatingMatrixAsItems() : mNumRating(0), mNumUser(0), mNumItem(0) { }
    RatingMatrixAsItems(RatingMatrixAsItems<ItemVectorAlloc>&& orig) noexcept :
            mNumRating(orig.mNumRating), mNumUser(orig.mNumUser),
            mNumItem(orig.mNumItem), mItemVectors(std::move(orig.mItemVectors)) { }
    void Init(RatingList& ratingList);
    int NumRating() const { return mNumRating; }
    int NumUser() const { return mNumUser; }
    int NumItem() const { return mNumItem; }
    double Sparsity() const {
        return 1.0 - static_cast<double>(mNumRating) / mNumUser / mNumItem;
    }
    const ItemVector<ItemVectorAlloc>& GetItemVector(int itemId) const {
        assert(itemId >= 0 && itemId < mItemVectors.size());
        return mItemVectors[itemId];
    }
    ItemVector<ItemVectorAlloc>& GetItemVector(int itemId) {
        assert(itemId >= 0 && itemId < mItemVectors.size());
        return mItemVectors[itemId];
    }
protected:
    int mNumRating;
    int mNumUser;
    int mNumItem;
    std::vector<ItemVector<ItemVectorAlloc>> mItemVectors;
    DISALLOW_COPY_AND_ASSIGN(RatingMatrixAsItems);
};

template <class ItemVectorAlloc>
void RatingMatrixAsItems<ItemVectorAlloc>::Init(RatingList& ratingList) { // RatingList is not CONST!
    QuickSortMT sort;
    sort(ratingList.Data(), ratingList.Size(),
        [](const RatingRecord& lhs, const RatingRecord& rhs) -> int {
            if (lhs.ItemId() != rhs.ItemId()) {
                return lhs.ItemId() - rhs.ItemId();
            } else {
                return lhs.UserId() - rhs.UserId();
            }
    });
    mNumRating = ratingList.Size();
    mNumUser = ratingList.NumUser();
    mNumItem = ratingList.NumItem();
    mItemVectors.resize(mNumItem);
    for (int i = 0; i < mNumRating; ++i) {
        int itemId = ratingList[i].ItemId();
        int begin = i;
        while (i+1 < mNumRating && ratingList[i].ItemId() == ratingList[i+1].ItemId()) {
            ++i;
        }
        int size = i - begin + 1;
        mItemVectors[itemId] = std::move(ItemVector<ItemVectorAlloc>(itemId, &ratingList[begin], size));
    }
}

typedef RatingMatrixAsItems<> RatingMatItems;

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_MATRIX_AS_ITEMS_H */
