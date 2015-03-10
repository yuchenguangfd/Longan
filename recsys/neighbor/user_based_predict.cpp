/*
 * user_based_predict.cpp
 * Created on: Nov 22, 2014
 * Author: chenguangyu
 */

#include "user_based_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/common.h"

namespace longan {

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
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
    mTrainDataItems = new RatingMatItems();
    mTrainDataItems->Init(rlist);
    if (mParameter->RatingType() == UserBased::Parameter::RatingType_Numerical) {
        Log::I("recsys", "adjust rating");
        mTrainDataTrait = new RatingTrait();
        mTrainDataTrait->Init(rlist);
        if (mParameter->SimType() == UserBased::Parameter::SimType_AdjustedCosine) {
            AdjustRatingByMinusItemAverage(*mTrainDataTrait, mTrainDataItems);
        } else if (mParameter->SimType() == UserBased::Parameter::SimType_Correlation) {
            AdjustRatingByMinusUserAverage(*mTrainDataTrait, mTrainDataItems);
        }
    }
}

void UserBasedPredict::LoadModel() {
    Log::I("recsys", "UserBasedPredict::LoadModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel = new UserBased::Model(mParameter, mTrainData->NumUser());
    mModel->Load(mModelFilepath);
}

float UserBasedPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mTrainDataItems->NumUser());
    assert(itemId >= 0 && itemId < mTrainDataItems->NumItem());
    if (mParameter->RatingType() == UserBased::Parameter::RatingType_Numerical) {
        if (mPredictOption->NeighborSize() <= 0) {
            return PredictRatingByAllNeighbor(userId, itemId);
        } else {
            return PredictRatingByFixedSizeNeighbor(userId, itemId);
        }
    } else if (mParameter->RatingType() == UserBased::Parameter::RatingType_Binary) {
         return 1.0f;
    } else {
        return 0.0f;
    }
}

float UserBasedPredict::PredictRatingByAllNeighbor(int userId, int itemId) const {
    const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
    double numerator = 0.0;
    double denominator = 0.0;
    for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
        float sim = mModel->Get(ur->UserId(), userId);
        numerator += sim * ur->Rating();
        denominator += Math::Abs(sim);
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == UserBased::Parameter::SimType_AdjustedCosine) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    } else if (mParameter->SimType() == UserBased::Parameter::SimType_Correlation) {
        predRating += mTrainDataTrait->UserAverage(userId);
    }
    return (float)predRating;
}

float UserBasedPredict::PredictRatingByFixedSizeNeighbor(int userId, int itemId) const {
    const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
    int neighborSize = Math::Min(mPredictOption->NeighborSize(), iv.Size());
    RunningMaxK<UserBased::NeighborUser> neighbors(neighborSize);
    for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
        float sim = mModel->Get(ur->UserId(), userId);
        neighbors.Add(UserBased::NeighborUser(sim, ur->Rating()));
    }
    double numerator = 0.0;
    double denominator = 0.0;
    for (const UserBased::NeighborUser *nu = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
            nu != end; ++nu) {
        numerator += nu->Similarity() * nu->Rating();
        denominator += Math::Abs(nu->Similarity());
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == UserBased::Parameter::SimType_AdjustedCosine) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    } else if (mParameter->SimType() == UserBased::Parameter::SimType_Correlation) {
        predRating += mTrainDataTrait->UserAverage(userId);
    }
    return (float)predRating;
}

float UserBasedPredict::ComputeTopNItemScore(int userId, int itemId) const {
    if (mPredictOption->PredictRankingMethod() == UserBased::PredictOption::PredictRankingMethod_PredictRating) {
        if (mPredictOption->NeighborSize() <= 0) {
            return PredictRatingByAllNeighbor(userId, itemId);
        } else {
            return PredictRatingByFixedSizeNeighbor(userId, itemId);
        }
    } else if (mPredictOption->PredictRankingMethod() == UserBased::PredictOption::PredictRankingMethod_NeighborSimilarity) {
        if (mPredictOption->NeighborSize() <= 0) {
            return ComputeTopNItemScoreByAllNeighbor(userId, itemId);
        } else {
            return ComputeTopNItemScoreByFixedSizeNeighbor(userId, itemId);
        }
    } else {
        return 0.0f;
    }
}

float UserBasedPredict::ComputeTopNItemScoreByAllNeighbor(int userId, int itemId) const {
    const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
    float score = 0.0f;
    for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
        float sim = mModel->Get(ur->UserId(), userId);
        score += sim;
    }
    return score;
}

float UserBasedPredict::ComputeTopNItemScoreByFixedSizeNeighbor(int userId, int itemId) const {
    const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
    int neighborSize = Math::Min(mPredictOption->NeighborSize(), iv.Size());
    RunningMaxK<float> neighbors(neighborSize);
    for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
        float sim = mModel->Get(ur->UserId(), userId);
        neighbors.Add(sim);
    }
    float score = 0.0f;
    for (const float *nu = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
            nu != end; ++nu) {
        score += *nu;
    }
    return score;
}

void UserBasedPredict::Cleanup() {
    Log::I("recsys", "UserBasedPredict::Cleanup()");
    delete mPredictOption;
    delete mParameter;
    delete mTrainDataItems;
    delete mTrainData;
    delete mTrainDataTrait;
    delete mModel;
}

} //~ namespace longan
