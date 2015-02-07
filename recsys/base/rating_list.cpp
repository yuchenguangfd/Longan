/*
 * rating_list.cpp
 * Created on: Oct 14, 2014
 * Author: chenguangyu
 */

#include "rating_list.h"
#include "common/lang/text_input_stream.h"
#include "common/lang/text_output_stream.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"
#include <cassert>

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

RatingList RatingList::LoadFromTextFile(const std::string& filename) {
    TextInputStream tis(filename);
    char sep;
    int numRating, numUser, numItem;
    tis >> numRating >> sep >> numUser >> sep >> numItem >> sep;
    RatingList ratingList(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
       int userId, itemId;
       float rating;
       int64 timestamp;
       tis >> userId >> sep >> itemId >> sep >> rating >> sep >> timestamp >> sep;
       ratingList.Add(RatingRecord(userId, itemId, rating)); // TODO discard timestamp for now
    }
    return std::move(ratingList);
}

RatingList RatingList::LoadFromBinaryFile(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numRating, numUser, numItem;
    bis >> numRating >> numUser >> numItem;
    RatingList ratingList(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
        int userId, itemId;
        float rating;
        bis >> userId >> itemId >> rating;
        ratingList.Add(RatingRecord(userId, itemId, rating));
    }
    return std::move(ratingList);
}

void RatingList::WriteToTextFile(const RatingList& rlist, const std::string& filename) {
    TextOutputStream tos(filename);
    tos << rlist.NumRating() << ',' << rlist.NumUser() << ','
        << rlist.NumItem() << '\n';
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        tos << rr.UserId() << ',' << rr.ItemId() << ','
            << rr.Rating() << ',' << 0LL <<'\n';
    }
}

void RatingList::WriteToBinaryFile(const RatingList& rlist, const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << rlist.NumRating() << rlist.NumUser() << rlist.NumItem();
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        bos << rr.UserId() << rr.ItemId() << rr.Rating();
    }
}

} //~ namespace longan
