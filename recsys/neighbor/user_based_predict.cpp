/*
 * user_based_predict.cpp
 * Created on: Nov 22, 2014
 * Author: chenguangyu
 */

#include "user_based_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/error.h"
#include <cassert>

namespace longan {

void UserBasedPredict::Init() {
    LoadConfig();
    LoadRatings();
    AdjustRating();
    LoadModel();
}

void UserBasedPredict::Cleanup() {
    delete mModel;
    delete mRatingMatrixAsItems;
    delete mRatingMatrixAsUsers;
    delete mRatingTrait;
}

void UserBasedPredict::LoadRatings() {
    Log::I("recsys", "UserBasedPredict::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix(as items)");
    mRatingMatrixAsItems = new RatingMatrixAsItems<>();
    mRatingMatrixAsItems->Init(rlist);
    Log::I("recsys", "create rating matrix(as users)");
    mRatingMatrixAsUsers = new RatingMatrixAsUsers<>();
    mRatingMatrixAsUsers->Init(rlist);
    Log::I("recsys", "create rating trait");
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(rlist);
}

void UserBasedPredict::AdjustRating() {
    if (mConfig["similarityType"].asString() == "adjustedCosine") {
        mSimType = SIM_TYPE_ADJUSTED_COSINE;
        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrixAsUsers);
        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrixAsItems);
    } else if (mConfig["similarityType"].asString() == "correlation") {
        mSimType = SIM_TYPE_CORRELATION;
        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrixAsUsers);
        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrixAsItems);
    } else { // (mConfig["similarityType"].asString == "cosine")
        mSimType = SIM_TYPE_COSINE;
    }
}

void UserBasedPredict::LoadModel() {
    Log::I("recsys", "UserBasedPredict::LoadModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel = new UserBased::ModelPredict();
    mModel->Load(mModelFilepath);
}

float UserBasedPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mRatingMatrixAsItems->NumUser());
    assert(itemId >= 0 && itemId < mRatingMatrixAsItems->NumItem());
    const auto& iv = mRatingMatrixAsItems->GetItemVector(itemId);
    const UserRating *data1 = iv.Data();
    int size1 = iv.Size();
    const UserBased::NeighborUser* data2 = mModel->NeighborBegin(userId);
    int size2 = mModel->NeighborSize(userId);
    double numerator = 0.0;
    double denominator = 0.0;
    if (size1 < size2) {
        for (int i = 0; i < size1; ++i) {
            const UserRating& ur = data1[i];
            int pos = BSearch(ur.UserId(), data2, size2,
                    [](int lhs, const UserBased::NeighborUser& rhs)->int {
                return lhs - rhs.UserId();
            });
            if (pos >= 0) {
                const UserBased::NeighborUser& nu = data2[pos];
                numerator += nu.Similarity() * ur.Rating();
                denominator += Math::Abs(nu.Similarity());
            }
        }
    } else {
        for (int i = 0; i < size2; ++i) {
            const UserBased::NeighborUser& nu = data2[i];
            int pos = BSearch(nu.UserId(), data1, size1,
                    [](int lhs, const UserRating& rhs)->int {
                return lhs - rhs.UserId();
            });
            if (pos >= 0) {
                const UserRating& ur = data1[pos];
                numerator += nu.Similarity() * ur.Rating();
                denominator += Math::Abs(nu.Similarity());
            }
        }
    }
    double predictedRating = numerator / (denominator + Double::EPS);
    if (mSimType == SIM_TYPE_ADJUSTED_COSINE) {
        predictedRating += mRatingTrait->ItemAverage(itemId);
    } else if (mSimType == SIM_TYPE_CORRELATION) {
        predictedRating += mRatingTrait->UserAverage(userId);
    }
    return (float)predictedRating;
}

ItemIdList UserBasedPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mRatingMatrixAsItems->NumUser());
    assert(listSize >= 0);
    int numItem = mRatingMatrixAsItems->NumItem();
    std::vector<double> numerators(numItem);
    std::vector<double> denominator(numItem);
    const UserBased::NeighborUser *data1 = mModel->NeighborBegin(userId);
    int size1 = mModel->NeighborSize(userId);
    for (int i = 0; i < size1; ++i) {
        int uid = data1[i].UserId();
        float sim = data1[i].Similarity();
        const auto& uv = mRatingMatrixAsUsers->GetUserVector(uid);
        const ItemRating *data2 = uv.Data();
        int size2 = uv.Size();
        for (int j = 0; j < size2; ++j) {
            int idx = data2[j].ItemId();
            numerators[idx] += data2[j].Rating()*sim;
            denominator[idx] += Math::Abs(sim);
        }
    }
    std::vector<ItemRating> ratings;
    ratings.reserve(numItem);
    const auto& uv = mRatingMatrixAsUsers->GetUserVector(userId);
    const ItemRating *data3 = uv.Data();
    int size3 = uv.Size();
    int begin = -1, end = -1;
    for (int i = 0; i < size3; ++i) {
        begin = end + 1;
        end = data3[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            float predRating = numerators[iid] / (denominator[iid] + Double::EPS);
            if (mSimType == SIM_TYPE_ADJUSTED_COSINE) {
                predRating += mRatingTrait->ItemAverage(iid);
            }
            ratings.push_back(ItemRating(iid, predRating));
        }
    }
    std::sort(ratings.begin(), ratings.end(),
            [](const ItemRating& lhs, const ItemRating& rhs)->bool {
                return lhs.Rating() > rhs.Rating();
    });
    ItemIdList resultList(listSize);
    if (listSize <= ratings.size()) {
        for (int i = 0; i < listSize; ++i) {
            resultList[i] = ratings[i].ItemId();
        }
    } else {
        for (int i = 0; i < ratings.size(); ++i) {
            resultList[i] = ratings[i].ItemId();
        }
        for (int i = ratings.size(); i < listSize; ++i) {
            resultList[i] = -1;
        }
    }
    return std::move(resultList);
}

} //~ namespace longan
