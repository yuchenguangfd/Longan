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
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace longan {

template <class ItemVectorAlloc = std::allocator<UserRating>>
class RatingMatrixAsItems {
public:
    RatingMatrixAsItems();
    RatingMatrixAsItems(const RatingMatrixAsItems<ItemVectorAlloc>& orig) = delete;
    RatingMatrixAsItems(RatingMatrixAsItems<ItemVectorAlloc>&& orig) noexcept;
    void Init(RatingList& ratingList);
    int NumUser() const {
        return mNumUser;
    }
    int NumItem() const {
        return mNumItem;
    }
    const ItemVector<ItemVectorAlloc>& GetItemVector(int itemId) const {
        assert(itemId >= 0 && itemId < mNumItem);
        return mItemVectors[itemId];
    }
    ItemVector<ItemVectorAlloc>& GetItemVector(int itemId) {
        assert(itemId >= 0 && itemId < mNumItem);
        return mItemVectors[itemId];
    }
protected:
    int mNumUser;
    int mNumItem;
    std::vector<ItemVector<ItemVectorAlloc>> mItemVectors;
};

template <class ItemVectorAlloc>
RatingMatrixAsItems<ItemVectorAlloc>::RatingMatrixAsItems() :
    mNumUser(0), mNumItem(0) { }

template <class ItemVectorAlloc>
RatingMatrixAsItems<ItemVectorAlloc>::RatingMatrixAsItems(RatingMatrixAsItems<ItemVectorAlloc>&& orig) noexcept:
    mNumUser(orig.mNumUser),
    mNumItem(orig.mNumItem),
    mItemVectors(std::move(orig.mItemVectors)) {
    orig.mNumUser = 0;
    orig.mNumItem = 0;
}

template <class ItemVectorAlloc>
void RatingMatrixAsItems<ItemVectorAlloc>::Init(RatingList& ratingList) {
    QuickSortMT sort;
    sort(&ratingList[0], ratingList.Size(),[](const RatingRecord& lhs, const RatingRecord& rhs) -> int {
        if (lhs.ItemId() != rhs.ItemId()) {
            return lhs.ItemId() - rhs.ItemId();
        } else {
            return lhs.UserId() - rhs.UserId();
        }
    });

    mNumUser = ratingList.NumUser();
    mNumItem = ratingList.NumItem();
    mItemVectors.resize(mNumItem);
    int numRating = ratingList.Size();
    for (int i = 0; i < numRating; ++i) {
        int itemId = ratingList[i].ItemId();
        int begin = i;
        while (i+1 < numRating && ratingList[i].ItemId() == ratingList[i+1].ItemId()) {
            ++i;
        }
        int size = i - begin + 1;
        mItemVectors[itemId] = std::move(ItemVector<ItemVectorAlloc>(itemId, &ratingList[begin], size));
    }
}

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_MATRIX_AS_ITEMS_H */
