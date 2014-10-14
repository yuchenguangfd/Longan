/*
 * user_vector.h
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_USER_VECTOR_H
#define RECSYS_BASE_USER_VECTOR_H

#include "item_rating.h"
#include "rating_record.h"
#include <memory>

namespace longan {

// all items' ratings for one user
template <class Alloc = std::allocator<ItemRating> >
class UserVector {
public:
    UserVector();
    UserVector(int userId, const RatingRecord *data, int size);
    ~UserVector();
protected:
    int mUserId;
    ItemRating *mData;
    int mSize;
};

template <class Alloc>
UserVector<Alloc>::UserVector() :
    mUserId(0),
    mData(nullptr),
    mSize(0) {}

template <class Alloc>
UserVector<Alloc>::UserVector(int userId, const RatingRecord *data, int size) {
    mUserId = userId;
    mSize = size;
    Alloc alloc;
    mData = alloc.allocate(mSize);
    for (int i = 0; i < size; ++i) {
        alloc.construct(&mData[i], ItemRating(data[i].ItemId(), data[i].Rating()));
    }
}

template <class Alloc>
UserVector<Alloc>::~UserVector() {
    Alloc alloc;
    alloc.deallocate(mData, mSize);
}

} //~ namespace longan

#endif /* RECSYS_BASE_USER_VECTOR_H */
