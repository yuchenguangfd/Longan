/*
 * rating_adjust.h
 * Created on: Dec 18, 2014
 * Author: chenguangyu
 */

#include "rating_trait.h"
#include "rating_matrix_as_users.h"
#include "rating_matrix_as_items.h"

namespace longan {

void AdjustRatingByMinusUserAverage(const RatingTrait& ratingTrait, RatingMatrixAsUsers<> *ratingMatrix);
void AdjustRatingByMinusItemAverage(const RatingTrait& ratingTrait, RatingMatrixAsUsers<> *ratingMatrix);
void AdjustRatingByMinusUserAverage(const RatingTrait& ratingTrait, RatingMatrixAsItems<> *ratingMatrix);
void AdjustRatingByMinusItemAverage(const RatingTrait& ratingTrait, RatingMatrixAsItems<> *ratingMatrix);

void AdjustRatingByNormalizeUserMaxAbs(RatingMatrixAsUsers<> *ratingMatrix);

} //~ namespace longan
