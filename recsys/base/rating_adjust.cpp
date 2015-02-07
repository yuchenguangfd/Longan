/*
 * rating_adjust.cpp
 * Created on: Dec 18, 2014
 * Author: chenguangyu
 */

#include "rating_adjust.h"

namespace longan {

void AdjustRatingByMinusUserAverage(const RatingTrait& rtrait, RatingMatUsers *rmat) {
    for (int uid = 0; uid < rmat->NumUser(); ++uid) {
         UserVec& uvec = rmat->GetUserVector(uid);
         float uavg = rtrait.UserAverage(uid);
         ItemRating *data = uvec.Data();
         int size = uvec.Size();
         for (int i = 0; i < size; ++i) {
             ItemRating& ir = data[i];
             ir.SetRating(ir.Rating() - uavg);
         }
     }
}

void AdjustRatingByMinusUserAverage(const RatingTrait& rtrait, RatingMatItems *rmat) {
    for (int iid = 0; iid < rmat->NumItem(); ++iid) {
        ItemVec& ivec = rmat->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        for (int i = 0; i < size; ++i) {
            UserRating& ur = data[i];
            float uavg = rtrait.UserAverage(ur.UserId());
            ur.SetRating(ur.Rating() - uavg);
       }
    }
}

void AdjustRatingByMinusItemAverage(const RatingTrait& rtrait, RatingMatUsers *rmat) {
    for (int uid = 0; uid < rmat->NumUser(); ++uid) {
        UserVec& uvec = rmat->GetUserVector(uid);
        ItemRating *data = uvec.Data();
        int size = uvec.Size();
        for (int i = 0; i < size; ++i) {
            ItemRating& ir = data[i];
            float iavg = rtrait.ItemAverage(ir.ItemId());
            ir.SetRating(ir.Rating() - iavg);
        }
    }
}

void AdjustRatingByMinusItemAverage(const RatingTrait& rtrait, RatingMatItems *rmat) {
    for (int iid = 0; iid < rmat->NumItem(); ++iid) {
        auto& ivec = rmat->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        float iavg = rtrait.ItemAverage(iid);
        for (int i = 0; i < size; ++i) {
            auto& ur = data[i];
            ur.SetRating(ur.Rating() - iavg);
        }
    }
}

} //~ namespace longan

