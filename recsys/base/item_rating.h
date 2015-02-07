/*
 * item_rating.h
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_ITEM_RATING_H
#define RECSYS_BASE_ITEM_RATING_H

namespace longan {

class ItemRating {
public:
	ItemRating() : mItemId(0), mRating(0.0) { }
	ItemRating(int itemId, float rating) : mItemId(itemId), mRating(rating) { }
	int ItemId() const { return mItemId; }
	void SetItemId(int itemId) { mItemId = itemId; }
	float Rating() const { return mRating; }
	void SetRating(float rating) { mRating = rating; }
protected:
	int mItemId;
	float mRating;
};

} //~ namespace longan

#endif /* RECSYS_BASE_ITEM_RATING_H */
