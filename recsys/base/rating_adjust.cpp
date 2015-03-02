/*
 * rating_adjust.cpp
 * Created on: Dec 18, 2014
 * Author: chenguangyu
 */

#include "rating_adjust.h"

namespace longan {

void AdjustRatingByMinusUserAverage(const RatingTrait& trait, RatingMatUsers *mat) {
    for (int uid = 0; uid < mat->NumUser(); ++uid) {
         UserVec& uvec = mat->GetUserVector(uid);
         float uavg = trait.UserAverage(uid);
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

void AdjustRatingByMinusUserAverage(RatingMatItems* mat) {
    std::vector<float> userRatingAvg(mat->NumUser());
    std::vector<int> userRatingCount(mat->NumUser());
    for (int iid = 0; iid < mat->NumItem(); ++iid) {
        const ItemVec& ivec = mat->GetItemVector(iid);
        const UserRating *data = ivec.Data();
        int size = ivec.Size();
        for (int i = 0; i < size; ++i) {
            const UserRating& ur = data[i];
            userRatingAvg[ur.UserId()] += ur.Rating();
            ++userRatingCount[ur.UserId()];
        }
    }
    for (int uid = 0; uid < mat->NumUser(); ++uid) {
        if (userRatingCount[uid] > 0) {
            userRatingAvg[uid] /= userRatingCount[uid];
        }
    }
    for (int iid = 0; iid < mat->NumItem(); ++iid) {
        ItemVec& ivec = mat->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        for (int i = 0; i < size; ++i) {
            UserRating& ur = data[i];
            ur.SetRating(ur.Rating() - userRatingAvg[ur.UserId()]);
        }
    }
}

void AdjustRatingByMinusUserAverage(RatingMatUsers* mat) {
    for (int uid = 0; uid < mat->NumUser(); ++uid) {
        UserVec& uvec = mat->GetUserVector(uid);
        ItemRating *data = uvec.Data();
        int size = uvec.Size();
        if (size == 0) continue;
        float uavg = 0.0;
        for (int i = 0; i < size; ++i) {
            uavg += data[i].Rating();
        }
        uavg /= size;
        for (int i = 0; i < size; ++i) {
            ItemRating &ir = data[i];
            ir.SetRating(ir.Rating() - uavg);
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
        ItemVec& ivec = rmat->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        float iavg = rtrait.ItemAverage(iid);
        for (int i = 0; i < size; ++i) {
            auto& ur = data[i];
            ur.SetRating(ur.Rating() - iavg);
        }
    }
}

void AdjustRatingByMinusItemAverage(RatingMatItems* mat) {
    for (int iid = 0; iid < mat->NumItem(); ++iid) {
        ItemVec& ivec = mat->GetItemVector(iid);
        UserRating *data = ivec.Data();
        int size = ivec.Size();
        if (size == 0) continue;
        float iavg = 0.0;
        for (int i = 0; i < size; ++i) {
            iavg += data[i].Rating();
        }
        iavg /= size;
        for (int i = 0; i < size; ++i) {
            UserRating &ur = data[i];
            ur.SetRating(ur.Rating() - iavg);
        }
    }
}

void AdjustRatingByMinusItemAverage(RatingMatUsers* mat) {
    std::vector<float> itemRatingAvg(mat->NumItem());
    std::vector<int> itemRatingCount(mat->NumItem());
    for (int uid = 0; uid < mat->NumUser(); ++uid) {
        const UserVec& uvec = mat->GetUserVector(uid);
        const ItemRating *data = uvec.Data();
        int size = uvec.Size();
        for (int i = 0; i < size; ++i) {
            const ItemRating& ir = data[i];
            itemRatingAvg[ir.ItemId()] += ir.Rating();
            ++itemRatingCount[ir.ItemId()];
        }
    }
    for (int iid = 0; iid < mat->NumItem(); ++iid) {
        if (itemRatingCount[iid] > 0) {
            itemRatingAvg[iid] /= itemRatingCount[iid];
        }
    }
    for (int uid = 0; uid < mat->NumUser(); ++uid) {
        UserVec& uvec = mat->GetUserVector(uid);
        ItemRating *data = uvec.Data();
        int size = uvec.Size();
        for (int i = 0; i < size; ++i) {
            ItemRating& ir = data[i];
            ir.SetRating(ir.Rating() - itemRatingAvg[ir.ItemId()]);
        }
    }
}

} //~ namespace longan
