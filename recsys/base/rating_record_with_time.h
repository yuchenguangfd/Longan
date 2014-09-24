/*
 * rating_record_with_time.h
 * Created on: Aug 9, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_UTIL_RATING_RECORD_WITH_TIME_H
#define RECSYS_UTIL_RATING_RECORD_WITH_TIME_H

#include "rating_record.h"

namespace longan {

class RatingRecordWithTime : public RatingRecord {
public:
    RatingRecordWithTime() : RatingRecord(), mTimestamp(0) { }
    RatingRecordWithTime(int userId, int itemId, float rating, int timestamp) :
        RatingRecord(userId, itemId, rating), mTimestamp(timestamp) { }
    int Timestamp() const {
        return mTimestamp;
    }
    void SetTimestamp(int timestamp) {
        mTimestamp = timestamp;
    }
private:
    int mTimestamp;
};

} //~ namespace longan

#endif /* RATING_RECORD_WITH_TIME_H_ */
