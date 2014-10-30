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
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace longan {

template <class UserVectorAlloc = std::allocator<ItemRating> >
class RatingMatrixAsUsers {
public:
    RatingMatrixAsUsers();
    void Init(RatingList& ratingList);
    int NumUser() const {
        return mNumUser;
    }
    int NumItem() const {
        return mNumItem;
    }
    const UserVector<UserVectorAlloc>& GetUserVector(int userId) const {
        assert(userId >= 0 && userId < mNumUser);
        return mUserVectors[userId];
    }
    UserVector<UserVectorAlloc>& GetUserVector(int userId) {
        assert(userId >= 0 && userId < mNumUser);
        return mUserVectors[userId];
    }
protected:
    int mNumUser;
    int mNumItem;
    std::vector<UserVector<UserVectorAlloc> > mUserVectors;
};

template <class UserVectorAlloc>
RatingMatrixAsUsers<UserVectorAlloc>::RatingMatrixAsUsers() :
    mNumUser(0), mNumItem(0) { }

template <class UserVectorAlloc>
void RatingMatrixAsUsers<UserVectorAlloc>::Init(RatingList& ratingList) {
    QuickSortMT sort;
    sort(&ratingList[0], ratingList.Size(), [](const RatingRecord& lhs, const RatingRecord& rhs) -> int {
       if (lhs.UserId() != rhs.UserId()) {
           return lhs.UserId() - rhs.UserId();
       } else {
           return lhs.ItemId() - rhs.ItemId();
       }
    });

    mNumUser = ratingList.NumUser();
    mNumItem = ratingList.NumItem();
    mUserVectors.resize(mNumUser);
    int numRating = ratingList.Size();
    for (int i = 0; i < numRating; ++i) {
        int userId = ratingList[i].UserId();
        int begin = i;
        while (i+1 < numRating && ratingList[i].UserId() == ratingList[i+1].UserId()) {
            ++i;
        }
        int size = i - begin + 1;
        mUserVectors[userId] = std::move(UserVector<UserVectorAlloc>(userId, &ratingList[begin], size));
    }
}

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_MATRIX_AS_USERS_H */
