/*
 * recsys_util.h
 * Created on: Nov 10, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_UTIL_RECSYS_UTIL_H
#define RECSYS_UTIL_RECSYS_UTIL_H

#include "recsys/base/rating_list.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_matrix_as_users.h"

namespace longan {

namespace RecsysUtil {

RatingList RandomRatingList(int numUser, int numItem, int numRating,
        bool isBinaryRating = false, float minRating = 0.0f, float maxRating = 1.0f);
RatingMatItems RandomRatingMatItems(int numUser, int numItem, int numRating,
        bool isBinaryRating = false, float minRating = 0.0f, float maxRating = 1.0f);
RatingMatUsers RandomRatingMatUsers(int numUser, int numItem, int numRating,
        bool isBinaryRating = false, float minRating = 0.0f, float maxRating = 1.0f);

} //~ namespace RecsysUtil

} //~ namespace longan

#endif /* RECSYS_UTIL_RECSYS_UTIL_H */
