/*
 * rating_record_with_time.h
 * Created on: Aug 9, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_RECORD_WITH_TIME_H
#define RECSYS_BASE_RATING_RECORD_WITH_TIME_H

#include "rating_record.h"
#include "common/lang/types.h"

namespace longan {

class RatingRecordWithTime : public RatingRecord {
public:
    RatingRecordWithTime() : RatingRecord(), mTimestamp(0LL) { }
    RatingRecordWithTime(int userId, int itemId, float rating, int64 timestamp) :
        RatingRecord(userId, itemId, rating), mTimestamp(timestamp) { }
    int64 Timestamp() const { return mTimestamp; }
    void SetTimestamp(int64 timestamp) { mTimestamp = timestamp; }
private:
    int64 mTimestamp;
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_RECORD_WITH_TIME_H */
