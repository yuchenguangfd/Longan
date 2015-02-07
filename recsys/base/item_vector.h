/*
 * item_vector.h
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_ITEM_VECTOR_H
#define RECSYS_BASE_ITEM_VECTOR_H

#include "user_rating.h"
#include "rating_record.h"
#include "common/lang/defines.h"
#include <memory>
#include <cassert>

namespace longan {

// all users' ratings for one item
template <class Alloc = std::allocator<UserRating>>
class ItemVector {
public:
    ItemVector();
    ItemVector(int itemId, const RatingRecord *data, int size);
    ItemVector(ItemVector<Alloc>&& orig) noexcept;
    ~ItemVector();
    ItemVector<Alloc>& operator= (ItemVector<Alloc>&& rhs) noexcept;
    int ItemId() const { return mItemId; }
    const UserRating* Data() const { return mData; }
    UserRating* Data() { return mData; }
    int Size() const { return mSize; }
    const UserRating* Begin() const { return mData; }
    UserRating* Begin() { return mData; }
    const UserRating* End() const { return mData + mSize; }
    UserRating* End() { return mData + mSize; }
protected:
    int mItemId;
    UserRating *mData;
    int mSize;
    DISALLOW_COPY_AND_ASSIGN(ItemVector);
};

template <class Alloc>
inline ItemVector<Alloc>::ItemVector() :
    mItemId(0),
    mData(nullptr),
    mSize(0) { }

template <class Alloc>
ItemVector<Alloc>::ItemVector(int itemId, const RatingRecord *data, int size) :
    mItemId(itemId),
    mSize(size) {
    Alloc alloc;
    mData = alloc.allocate(mSize);
    for (int i = 0; i < size; ++i) {
        assert(data[i].ItemId() == mItemId);
        alloc.construct(&mData[i], UserRating(data[i].UserId(), data[i].Rating()));
    }
}

template <class Alloc>
ItemVector<Alloc>::ItemVector(ItemVector<Alloc>&& orig) noexcept :
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
ItemVector<Alloc>& ItemVector<Alloc>::operator= (ItemVector<Alloc>&& rhs) noexcept {
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

typedef ItemVector<std::allocator<UserRating>> ItemVec;

} //~ namespace longan

#endif /* RECSYS_BASE_ITEM_VECTOR_H */
