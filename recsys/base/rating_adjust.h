/*
 * rating_adjust.h
 * Created on: Dec 18, 2014
 * Author: chenguangyu
 */

#include "rating_trait.h"
#include "rating_matrix_as_users.h"
#include "rating_matrix_as_items.h"

namespace longan {

void AdjustRatingByMinusUserAverage(const RatingTrait& rtrait, RatingMatUsers *rmat);
void AdjustRatingByMinusUserAverage(const RatingTrait& rtrait, RatingMatItems *rmat);

void AdjustRatingByMinusItemAverage(const RatingTrait& rtrait, RatingMatUsers *rmat);
void AdjustRatingByMinusItemAverage(const RatingTrait& rtrait, RatingMatItems *rmat);

} //~ namespace longan
