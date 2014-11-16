/*
 * rating_list.h
 * Created on: Oct 6, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_LIST_H
#define RECSYS_BASE_RATING_LIST_H

#include "rating_record.h"
#include <memory>
#include <string>
#include <vector>

#include "common/lang/defines.h"

namespace longan {

class RatingList {
public:
    RatingList();
    RatingList(int numUser, int numItem);
    RatingList(int numUser, int numItem, int reservedCapacity);
    RatingList(RatingList&& orig) noexcept;
    ~RatingList();
    RatingList& operator= (RatingList&& rhs) noexcept;
    void Add(const RatingRecord& record) {
        mRatingRecords.push_back(record);
    }
    int NumUser() const { 
        return mNumUser;
    }
    int NumItem() const { 
        return mNumItem; 
    }
    int NumRating() const {
        return mRatingRecords.size();
    }
    int Size() const {
        return mRatingRecords.size();
    }
    const RatingRecord& operator[] (int i) const {
        return mRatingRecords[i];
    }
    RatingRecord& operator[] (int i) {
        return mRatingRecords[i];
    }
private:
    int mNumUser;
    int mNumItem;
    std::vector<RatingRecord> mRatingRecords;
    DISALLOW_COPY_AND_ASSIGN(RatingList);
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_LIST_H */
