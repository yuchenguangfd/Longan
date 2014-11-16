/*
 * rating_list.cpp
 * Created on: Oct 14, 2014
 * Author: chenguangyu
 */

#include "rating_list.h"
namespace longan {

RatingList::RatingList() :
    mNumUser(0),
    mNumItem(0) { }

RatingList::RatingList(int numUser, int numItem) :
    mNumUser(numUser),
    mNumItem(numItem) { }

RatingList::RatingList(int numUser, int numItem, int reservedCapacity) :
    mNumUser(numUser),
    mNumItem(numItem) {
    mRatingRecords.reserve(reservedCapacity);
}

RatingList::RatingList(RatingList&& orig) noexcept :
    mNumUser(orig.mNumUser),
    mNumItem(orig.mNumItem),
    mRatingRecords(std::move(orig.mRatingRecords)) {
}

RatingList::~RatingList() { }

RatingList& RatingList::operator= (RatingList&& rhs) noexcept {
    if (this == &rhs) return *this;
    mNumUser = rhs.mNumUser;
    mNumItem = rhs.mNumItem;
    mRatingRecords = std::move(rhs.mRatingRecords);
    return *this;
}

} //~ namespace longan


