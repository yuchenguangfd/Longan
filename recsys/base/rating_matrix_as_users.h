/*
 * rating_matrix_as_users.h
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_MATRIX_AS_USERS_H
#define RECSYS_BASE_RATING_MATRIX_AS_USERS_H

#include "rating_list.h"
#include "user_vector.h"
#include "algorithm/sort/quick_sort_mt.h"
#include "common/lang/defines.h"
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace longan {

template <class UserVectorAlloc = std::allocator<ItemRating>>
class RatingMatrixAsUsers {
public:
    RatingMatrixAsUsers() : mNumRating(0), mNumUser(0), mNumItem(0) { }
    RatingMatrixAsUsers(RatingMatrixAsUsers<UserVectorAlloc>&& orig) noexcept :
        mNumRating(orig.mNumRating), mNumUser(orig.mNumUser),
        mNumItem(orig.mNumItem), mUserVectors(std::move(orig.mUserVectors)) { }
    void Init(RatingList& ratingList);
    int NumRating() const { return mNumRating; }
    int NumUser() const { return mNumUser; }
    int NumItem() const { return mNumItem; }
    double Sparsity() const {
        return 1.0 - static_cast<double>(mNumRating) / mNumUser / mNumItem;
    }
    const UserVector<UserVectorAlloc>& GetUserVector(int userId) const {
        assert(userId >= 0 && userId < mUserVectors.size());
        return mUserVectors[userId];
    }
    UserVector<UserVectorAlloc>& GetUserVector(int userId) {
        assert(userId >= 0 && userId < mUserVectors.size());
        return mUserVectors[userId];
    }
protected:
    int mNumRating;
    int mNumUser;
    int mNumItem;
    std::vector<UserVector<UserVectorAlloc>> mUserVectors;
    DISALLOW_COPY_AND_ASSIGN(RatingMatrixAsUsers);
};

template <class UserVectorAlloc>
void RatingMatrixAsUsers<UserVectorAlloc>::Init(RatingList& ratingList) { // RatingList is not CONST!
    QuickSortMT sort;
    sort(ratingList.Data(), ratingList.Size(),
        [](const RatingRecord& lhs, const RatingRecord& rhs) -> int {
            if (lhs.UserId() != rhs.UserId()) {
                return lhs.UserId() - rhs.UserId();
            } else {
                return lhs.ItemId() - rhs.ItemId();
            }
    });
    mNumRating = ratingList.NumRating();
    mNumUser = ratingList.NumUser();
    mNumItem = ratingList.NumItem();
    mUserVectors.resize(mNumUser);
    for (int i = 0; i < mNumRating; ++i) {
        int userId = ratingList[i].UserId();
        int begin = i;
        while (i+1 < mNumRating && ratingList[i].UserId() == ratingList[i+1].UserId()) {
            ++i;
        }
        int size = i - begin + 1;
        mUserVectors[userId] = std::move(UserVector<UserVectorAlloc>(userId, &ratingList[begin], size));
    }
}

typedef RatingMatrixAsUsers<> RatingMatUsers;

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_MATRIX_AS_USERS_H */
