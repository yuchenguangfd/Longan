/*
 * recsys_test_helper.h
 * Created on: Nov 10, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_UTIL_RECSYS_TEST_HELPER_H
#define RECSYS_UTIL_RECSYS_TEST_HELPER_H

#include "recsys/base/rating_list.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_matrix_as_users.h"

namespace longan {

class RecsysTestHelper {
public:
    static RatingList CreateRandomRatingList(int numUser, int numItem, int numRating);
    static RatingMatrixAsItems<> CreateRandomRatingMatItems(int numUser, int numItem, int numRating);
    static RatingMatrixAsUsers<> CreateRandomRatingMatUsers(int numUser, int numItem, int numRating);
};

} //~ namespace longan

#endif /* RECSYS_UTIL_RECSYS_TEST_HELPER_H */
