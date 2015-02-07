/*
 * rating_record.h
 * Created on: Aug 8, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_RECORD_H
#define RECSYS_BASE_RATING_RECORD_H

namespace longan {

class RatingRecord {
public:
	RatingRecord() : mUserId(0), mItemId(0), mRating(0.0) { }
	RatingRecord(int userId, int itemId, float rating) : mUserId(userId), mItemId(itemId),
	        mRating(rating) { }
	int UserId() const { return mUserId; }
	void SetUserId(int userId) { mUserId = userId; }
	int ItemId() const { return mItemId; }
	void SetItemId(int itemId) { mItemId = itemId; }
	float Rating() const { return mRating; }
	void SetRating(float rating) { mRating = rating; }
protected:
	int mUserId;
	int mItemId;
	float mRating;
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_RECORD_H */
