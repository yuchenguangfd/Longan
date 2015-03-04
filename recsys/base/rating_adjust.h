/*
 * rating_adjust.h
 * Created on: Dec 18, 2014
 * Author: chenguangyu
 */

#include "rating_trait.h"
#include "rating_matrix_as_users.h"
#include "rating_matrix_as_items.h"

namespace longan {

void AdjustRatingByMinusUserAverage(const RatingTrait& trait, RatingMatUsers *mat);
void AdjustRatingByMinusUserAverage(const RatingTrait& trait, RatingMatItems *mat);
void AdjustRatingByMinusUserAverage(RatingMatItems *mat);
void AdjustRatingByMinusUserAverage(RatingMatUsers *mat);

void AdjustRatingByMinusItemAverage(const RatingTrait& trait, RatingMatUsers *mat);
void AdjustRatingByMinusItemAverage(const RatingTrait& trait, RatingMatItems *mat);
void AdjustRatingByMinusItemAverage(RatingMatItems *mat);
void AdjustRatingByMinusItemAverage(RatingMatUsers *mat);

void NormalizeRatingByDivideMax(RatingList *list);

} //~ namespace longan
