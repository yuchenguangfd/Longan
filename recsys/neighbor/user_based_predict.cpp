/*
 * user_based_predict.cpp
 * Created on: Nov 22, 2014
 * Author: chenguangyu
 */

#include "user_based_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/util/running_statistic.h"
#include <algorithm>
#include <cassert>

namespace longan {

void UserBasedPredict::Init() {
    Log::I("recsys", "UserBasedPredict::Init()");
    LoadConfig();
    CreatePredictOption();
    CreateParameter();
    LoadTrainData();
    AdjustRating();
    LoadModel();
    InitCachedTopNItems();
}

void UserBasedPredict::CreatePredictOption() {
    Log::I("recsys", "UserBasedPredict::CreatePredictOption()");
    mPredictOption = new UserBased::PredictOption(mConfig["predictOption"]);
}

void UserBasedPredict::CreateParameter() {
    Log::I("recsys", "UserBasedPredict::CreateParameter()");
    mParameter = new UserBased::Parameter(mConfig["parameter"]);
}

void UserBasedPredict::LoadTrainData() {
    Log::I("recsys", "UserBasedPredict::LoadTrainData()");
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainDataItems = new RatingMatItems();
    mTrainDataItems->Init(rlist);
    mTrainDataUsers = new RatingMatUsers();
    mTrainDataUsers->Init(rlist);
    mTrainDataTrait = new RatingTrait();
    mTrainDataTrait->Init(rlist);
}

void UserBasedPredict::AdjustRating() {
    Log::I("recsys", "UserBasedPredict::AdjustRating()");
    if (mParameter->SimType() == UserBased::Parameter::SimTypeAdjustedCosine) {
        AdjustRatingByMinusItemAverage(*mTrainDataTrait, mTrainDataItems);
    } else if (mParameter->SimType() == UserBased::Parameter::SimTypeCorrelation) {
        AdjustRatingByMinusUserAverage(*mTrainDataTrait, mTrainDataItems);
    }
}

void UserBasedPredict::LoadModel() {
    Log::I("recsys", "UserBasedPredict::LoadModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel = new UserBased::ModelPredict();
    mModel->Load(mModelFilepath);
}

void UserBasedPredict::InitCachedTopNItems() {
    Log::I("recsys", "UserBasedPredict::InitCachedTopNItems()");
    mCachedTopNItems.resize(mTrainDataItems->NumUser());
}

float UserBasedPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mTrainDataItems->NumUser());
    assert(itemId >= 0 && itemId < mTrainDataItems->NumItem());
    if (mPredictOption->NeighborSize() <= 0) {
        return PredictRatingAllNeighbor(userId, itemId);
    } else {
        return PredictRatingFixedSizeNeighbor(userId, itemId);
    }
}

float UserBasedPredict::PredictRatingAllNeighbor(int userId, int itemId) const {
    const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
    double numerator = 0.0;
    double denominator = 0.0;
    for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
        float sim = mModel->GetSimilarity(ur->UserId(), userId);
        numerator += sim * ur->Rating();
        denominator += Math::Abs(sim);
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == UserBased::Parameter::SimTypeAdjustedCosine) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    } else if (mParameter->SimType() == UserBased::Parameter::SimTypeCorrelation) {
        predRating += mTrainDataTrait->UserAverage(userId);
    }
    return (float)predRating;
}

float UserBasedPredict::PredictRatingFixedSizeNeighbor(int userId, int itemId) const {
    const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
    int neighborSize = Math::Min(mPredictOption->NeighborSize(), iv.Size());
    RunningMaxK<UserBased::NeighborUser> neighbors(neighborSize);
    for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
        float sim = mModel->GetSimilarity(ur->UserId(), userId);
        neighbors.Add(UserBased::NeighborUser(ur->UserId(), sim, ur->Rating()));
    }
    double numerator = 0.0;
    double denominator = 0.0;
    for (const UserBased::NeighborUser *nu = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
            nu != end; ++nu) {
        numerator += nu->Similarity() * nu->Rating();
        denominator += Math::Abs(nu->Similarity());
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == UserBased::Parameter::SimTypeAdjustedCosine) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    } else if (mParameter->SimType() == UserBased::Parameter::SimTypeCorrelation) {
        predRating += mTrainDataTrait->UserAverage(userId);
    }
    return (float)predRating;
}

ItemIdList UserBasedPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mTrainDataUsers->NumUser());
    assert(listSize >= 0);
    if (listSize <= mCachedTopNItems[userId].size()) {
        return PredictTopNItemFromCache(userId, listSize);
    }
    int numItem = mTrainDataUsers->NumItem();
    const UserVec& uv = mTrainDataUsers->GetUserVector(userId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    std::vector<ItemRating> ratings;
    ratings.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size; ++i) {
        begin = end = 1;
        end = data[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            float predRating = PredictRating(userId, iid);
            ratings.push_back(ItemRating(iid, predRating));
        }
    }
    begin = end + 1;
    end = numItem;
    for (int iid = begin; iid < end; ++iid) {
        float predRating = PredictRating(userId, iid);
        ratings.push_back(ItemRating(iid, predRating));
    }
    std::sort(ratings.begin(), ratings.end(),
            [](const ItemRating& lhs, const ItemRating& rhs)->bool {
                return lhs.Rating() > rhs.Rating();
    });
    ItemIdList topNItem(listSize);
    if (listSize <= ratings.size()) {
        for (int i = 0; i < listSize; ++i) {
            topNItem[i] = ratings[i].ItemId();
        }
    } else {
        for (int i = 0; i < ratings.size(); ++i) {
            topNItem[i] = ratings[i].ItemId();
        }
    }
    mCachedTopNItems[userId] = topNItem;
    return std::move(topNItem);
}

ItemIdList UserBasedPredict::PredictTopNItemFromCache(int userId, int listSize) const {
    ItemIdList topNItem(listSize);
    for (int i = 0; i < listSize; ++i) {
        topNItem[i] = mCachedTopNItems[userId][i];
    }
    return std::move(topNItem);
}

float UserBasedPredict::ComputeItemSimilarity(int itemId1, int itemId2) const {
    const ItemVec& iv1 = mTrainDataItems->GetItemVector(itemId1);
    const ItemVec& iv2 = mTrainDataItems->GetItemVector(itemId2);
    int size1 = iv1.Size();
    if (size1 == 0) return 0.0f;
    int size2 = iv2.Size();
    if (size2 == 0) return 0.0f;
    const UserRating* data1 = iv1.Data();
    const UserRating* data2 = iv2.Data();
    double sum = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;
    int i = 0, j = 0;
    int uid1 = data1[i].UserId();
    int uid2 = data2[j].UserId();
    double rating1 = data1[i].Rating();
    double rating2 = data2[j].Rating();
    while (true) {
        if (uid1 == uid2) {
            sum += rating1 * rating2;
            norm1 += rating1 * rating1;
            norm2 += rating2 * rating2;
            ++i; ++j;
            if (i == size1 || j == size2) break;
            uid1 = data1[i].UserId();
            rating1 = data1[i].Rating();
            uid2 = data2[j].UserId();
            rating2 = data2[j].Rating();
        } else if (uid1 < uid2) {
            ++i;
            if (i == size1) break;
            uid1 = data1[i].UserId();
            rating1 = data1[i].Rating();
        } else if (uid1 > uid2) {
            ++j;
            if (j == size2) break;
            uid2 = data2[j].UserId();
            rating2 = data2[j].Rating();
        }
    }
    double denominator = Math::Sqrt(norm1 * norm2);
    return (float)(sum / (denominator + Double::EPS));
}

void UserBasedPredict::Cleanup() {
    Log::I("recsys", "UserBasedPredict::Cleanup()");
    delete mPredictOption;
    delete mParameter;
    delete mTrainDataItems;
    delete mTrainDataUsers;
    delete mTrainDataTrait;
    delete mModel;
}

} //~ namespace longan
