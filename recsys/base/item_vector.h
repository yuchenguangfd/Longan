/*
 * item_vector.h
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_ITEM_VECTOR_H
#define RECSYS_BASE_ITEM_VECTOR_H

#include "user_rating.h"
#include "rating_record.h"
#include <memory>

namespace longan {

// all users' ratings for one item
template <class Alloc = std::allocator<UserRating> >
class ItemVector {
public:
    ItemVector();
    ItemVector(int itemId, const RatingRecord *data, int size);
    ItemVector(const ItemVector<Alloc>&) = delete;
    ItemVector(ItemVector<Alloc>&& orig) noexcept;
    ~ItemVector();
    ItemVector<Alloc>& operator= (const ItemVector<Alloc>& rhs) = delete;
    ItemVector<Alloc>& operator= (ItemVector<Alloc>&& rhs) noexcept;
    const UserRating* Begin() const {
        return mData;
    }
    const UserRating* End() const {
        return mData + mSize;
    }
protected:
    int mItemId;
    UserRating *mData;
    int mSize;
};

template <class Alloc>
ItemVector<Alloc>::ItemVector() :
    mItemId(0),
    mData(nullptr),
    mSize(0) { }

template <class Alloc>
ItemVector<Alloc>::ItemVector(int itemId, const RatingRecord *data, int size) {
    mItemId = itemId;
    mSize = size;
    Alloc alloc;
    mData = alloc.allocate(mSize);
    for (int i = 0; i < size; ++i) {
        alloc.construct(&mData[i], UserRating(data[i].UserId(), data[i].Rating()));
    }
}

template <class Alloc>
ItemVector<Alloc>::ItemVector(ItemVector&& orig) noexcept :
    mItemId(orig.mItemId),
    mData(orig.mData),
    mSize(orig.mSize) {
    orig.mItemId = 0;
    orig.mData = nullptr;
    orig.mSize = 0;
}

template <class Alloc>
ItemVector<Alloc>::~ItemVector() {
    Alloc alloc;
    alloc.deallocate(mData, mSize);
}

template <class Alloc>
ItemVector<Alloc>& ItemVector<Alloc>::operator =(ItemVector<Alloc>&& rhs) noexcept {
    if (this == &rhs) return *this;
    if (mData != nullptr) {
        Alloc alloc;
        alloc.deallocate(mData, mSize);
    }
    mItemId = rhs.mItemId;
    mData = rhs.mData;
    mSize = rhs.mSize;
    rhs.mItemId = 0;
    rhs.mData = nullptr;
    rhs.mSize = 0;
    return *this;
}

} //~ namespace longan

#endif /* RECSYS_BASE_ITEM_VECTOR_H */
