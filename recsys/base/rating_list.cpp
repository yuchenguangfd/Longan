/*
 * rating_list.cpp
 * Created on: Oct 14, 2014
 * Author: chenguangyu
 */

#include "rating_list.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"
#include <cstdio>
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

RatingList RatingList::LoadFromTextFile(const std::string& ratingTextFilepath) {
    int rtn;
    FILE *fp = fopen(ratingTextFilepath.c_str(), "r");
    assert(fp != nullptr);
    int numRating, numUser, numItem;
    rtn = fscanf(fp, "%d,%d,%d", &numRating, &numUser, &numItem);
    assert(rtn == 3);
    RatingList ratingList(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
       int userId, itemId, time;
       float rating;
       rtn = fscanf(fp, "%d,%d,%f,%d", &userId, &itemId, &rating, &time);
       assert(rtn == 4);
       ratingList.Add(RatingRecord(userId, itemId, rating));
    }
    fclose(fp);
    return std::move(ratingList);
}

RatingList longan::RatingList::LoadFromBinaryFile(const std::string& ratingBinaryFilepath) {
    BinaryInputStream bis(ratingBinaryFilepath);
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

void RatingList::WriteToTextFile(const RatingList& rlist, const std::string& ratingTextFilepath) {
    int rtn;
    FILE *fp = fopen(ratingTextFilepath.c_str(), "w");
    assert(fp != nullptr);
    rtn = fprintf(fp, "%d,%d,%d\n",rlist.NumRating(), rlist.NumUser(), rlist.NumItem());
    assert(rtn >= 0);
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        rtn = fprintf(fp,"%d,%d,%f,%d\n", rr.UserId(), rr.ItemId(), rr.Rating(), 0);
        assert(rtn >= 0);
    }
    fclose(fp);
}

void RatingList::WriteToBinaryFile(const RatingList& rlist, const std::string& ratingBinaryFilepath) {
    BinaryOutputStream bos(ratingBinaryFilepath);
    bos << rlist.NumRating() << rlist.NumUser() << rlist.NumItem();
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        bos << rr.UserId() << rr.ItemId() << rr.Rating();
    }
}

} //~ namespace longan
