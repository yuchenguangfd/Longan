/*
 * recsys_test_helper.h
 * Created on: Nov 10, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_UTIL_RECSYS_TEST_HELPER_H
#define RECSYS_UTIL_RECSYS_TEST_HELPER_H

#include "recsys/base/rating_list.h"
#include "recsys/base/rating_matrix_as_items.h"

namespace longan {

class RecsysTestHelper {
public:
    static RatingList CreateRandomRatingList(int numUser, int numItem, int numRating);
    static RatingMatrixAsItems<> CreateRandomRatingMatrixAsItems(int numUser, int numItem, int numRating);
};

} //~ namespace longan

#endif /* RECSYS_UTIL_RECSYS_TEST_HELPER_H */
