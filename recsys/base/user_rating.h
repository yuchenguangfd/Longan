/*
 * user_rating.h
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_USER_RATING_H
#define RECSYS_BASE_USER_RATING_H

namespace longan {

class UserRating {
public:
	UserRating() : mUserId(0), mRating(0.0) { }
	UserRating(int userId, float rating) : mUserId(userId), mRating(rating) { }
	int UserId() const { return mUserId; }
	void SetUserId(int userId) { mUserId = userId; }
	float Rating() const { return mRating; }
	void SetRating(float rating) { mRating = rating; }
protected:
	int mUserId;
	float mRating;
};

} //~ namespace longan

#endif /* RECSYS_BASE_USER_RATING_H */
