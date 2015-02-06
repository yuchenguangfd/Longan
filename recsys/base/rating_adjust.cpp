/*
 * rating_adjust.cpp
 * Created on: Dec 18, 2014
 * Author: chenguangyu
 */

#include "rating_adjust.h"
#include "common/math/math.h"

namespace longan {

void AdjustRatingByMinusUserAverage(const RatingTrait& ratingTrait, RatingMatrixAsUsers<> *ratingMatrix) {
    for (int uid = 0; uid < ratingMatrix->NumUser(); ++uid) {
         auto& uvec = ratingMatrix->GetUserVector(uid);
         float uavg = ratingTrait.UserAverage(uid);
         ItemRating *data = uvec.Data();
         int size = uvec.Size();
         for (int i = 0; i < size; ++i) {
             auto& ir = data[i];
             ir.SetRating(ir.Rating() - uavg);
         }
     }
}

void AdjustRatingByMinusItemAverage(const RatingTrait& ratingTrait, RatingMatrixAsUsers<> *ratingMatrix) {
    for (int uid = 0; uid < ratingMatrix->NumUser(); ++uid) {
        auto& uvec = ratingMatrix->GetUserVector(uid);
        ItemRating *data = uvec.Data();
        int size = uvec.Size();
        for (int i = 0; i < size; ++i) {
            auto& ir = data[i];
            float iavg = ratingTrait.ItemAverage(ir.ItemId());
            ir.SetRating(ir.Rating() - iavg);
        }
    }
}

void AdjustRatingByMinusUserAverage(const RatingTrait& ratingTrait, RatingMatrixAsItems<> *ratingMatrix) {
    for (int iid = 0; iid < ratingMatrix->NumItem(); ++iid) {
        auto& ivec = ratingMatrix->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        for (int i = 0; i < size; ++i) {
            auto& ur = data[i];
            float uavg = ratingTrait.UserAverage(ur.UserId());
            ur.SetRating(ur.Rating() - uavg);
       }
    }
}

void AdjustRatingByMinusItemAverage(const RatingTrait& ratingTrait, RatingMatrixAsItems<> *ratingMatrix) {
    for (int iid = 0; iid < ratingMatrix->NumItem(); ++iid) {
        auto& ivec = ratingMatrix->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        float iavg = ratingTrait.ItemAverage(iid);
        for (int i = 0; i < size; ++i) {
            auto& ur = data[i];
            ur.SetRating(ur.Rating() - iavg);
        }
    }
}

void AdjustRatingByNormalizeUserMaxAbs(RatingMatrixAsUsers<>* ratingMatrix) {
    for (int uid = 0; uid < ratingMatrix->NumUser(); ++uid) {
        auto& uvec = ratingMatrix->GetUserVector(uid);
        ItemRating *data = uvec.Data();
        int size = uvec.Size();
        float maxAbs = 1e-10;
        for (int i = 0; i < size; ++i) {
            maxAbs = Math::Max(Math::Abs(data[i].Rating()), maxAbs);
        }
        for (int i = 0; i < size; ++i) {
            auto& ir = data[i];
            ir.SetRating(ir.Rating() / maxAbs);
        }
    }
}

} //~ namespace longan

