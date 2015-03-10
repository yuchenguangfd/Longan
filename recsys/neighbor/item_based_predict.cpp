/*
 * item_based_predict.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/common.h"

namespace longan {

void ItemBasedPredict::CreatePredictOption() {
    Log::I("recsys", "ItemBasedPredict::CreatePredictOption()");
    mPredictOption = new ItemBased::PredictOption(mConfig["predictOption"]);
}

void ItemBasedPredict::CreateParameter() {
    Log::I("recsys", "ItemBasedPredict::CreateParameter()");
    mParameter = new ItemBased::Parameter(mConfig["parameter"]);
}

void ItemBasedPredict::LoadTrainData() {
    Log::I("recsys", "ItemBasedPredict::LoadTrainData()");
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
    if (mParameter->RatingType() == ItemBased::Parameter::RatingType_Numerical) {
        Log::I("recsys", "adjust rating");
        mTrainDataTrait = new RatingTrait();
        mTrainDataTrait->Init(rlist);
        if (mParameter->SimType() == ItemBased::Parameter::SimType_AdjustedCosine) {
            AdjustRatingByMinusUserAverage(*mTrainDataTrait, mTrainData);
        } else if (mParameter->SimType() == ItemBased::Parameter::SimType_Correlation) {
            AdjustRatingByMinusItemAverage(*mTrainDataTrait, mTrainData);
        }
    }
}

void ItemBasedPredict::LoadModel() {
    Log::I("recsys", "ItemBasedPredict::LoadModel()");
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel = new ItemBased::Model(mParameter, mTrainData->NumItem());
    mModel->Load(mModelFilepath);
}

float ItemBasedPredict::PredictRating(int userId, int itemId) const {
	assert(userId >= 0 && userId < mTrainData->NumUser());
	assert(itemId >= 0 && itemId < mTrainData->NumItem());
    if (mParameter->RatingType() == ItemBased::Parameter::RatingType_Numerical) {
        if (mPredictOption->NeighborSize() <= 0) {
            return PredictRatingByAllNeighbor(userId, itemId);
        } else {
            return PredictRatingByFixedSizeNeighbor(userId, itemId);
        }
    } else if (mParameter->RatingType() == ItemBased::Parameter::RatingType_Binary) {
        return 1.0f;
    } else {
        return 0.0f;
    }
}

float ItemBasedPredict::PredictRatingByAllNeighbor(int userId, int itemId) const {
    const UserVec& uv = mTrainData->GetUserVector(userId);
    double numerator = 0.0;
    double denominator = 0.0;
    for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
        float sim = mModel->Get(ir->ItemId(), itemId);
        numerator += sim * ir->Rating();
        denominator += Math::Abs(sim);
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == ItemBased::Parameter::SimType_AdjustedCosine) {
        predRating += mTrainDataTrait->UserAverage(userId);
    } else if (mParameter->SimType() == ItemBased::Parameter::SimType_Correlation) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    }
    return (float)predRating;
}

float ItemBasedPredict::PredictRatingByFixedSizeNeighbor(int userId, int itemId) const {
    const UserVec& uv = mTrainData->GetUserVector(userId);
    int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
    RunningMaxK<ItemBased::NeighborItem> neighbors(neighborSize);
    for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
        float sim = mModel->Get(ir->ItemId(), itemId);
        neighbors.Add(ItemBased::NeighborItem(sim, ir->Rating()));
    }
    double numerator = 0.0;
    double denominator = 0.0;
    for (const ItemBased::NeighborItem *ni = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
            ni != end; ++ni) {
        numerator += ni->Similarity() * ni->Rating();
        denominator += Math::Abs(ni->Similarity());
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == ItemBased::Parameter::SimType_AdjustedCosine) {
        predRating += mTrainDataTrait->UserAverage(userId);
    } else if (mParameter->SimType() == ItemBased::Parameter::SimType_Correlation) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    }
    return (float)predRating;
}

float ItemBasedPredict::ComputeTopNItemScore(int userId, int itemId) const {
    if (mPredictOption->PredictRankingMethod() == ItemBased::PredictOption::PredictRankingMethod_PredictRating) {
        if (mPredictOption->NeighborSize() <= 0) {
            return PredictRatingByAllNeighbor(userId, itemId);
        } else {
            return PredictRatingByFixedSizeNeighbor(userId, itemId);
        }
    } else if (mPredictOption->PredictRankingMethod() == ItemBased::PredictOption::PredictRankingMethod_NeighborSimilarity) {
        if (mPredictOption->NeighborSize() <= 0) {
            return ComputeTopNItemScoreByAllNeighbor(userId, itemId);
        } else {
            return ComputeTopNItemScoreByFixedSizeNeighbor(userId, itemId);
        }
    } else {
        return 0.0f;
    }
}

float ItemBasedPredict::ComputeTopNItemScoreByAllNeighbor(int userId, int itemId) const {
    const UserVec& uv = mTrainData->GetUserVector(userId);
    float score = 0.0f;
    for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
        float sim = mModel->Get(ir->ItemId(), itemId);
        score += sim;
    }
    return score;
}

float ItemBasedPredict::ComputeTopNItemScoreByFixedSizeNeighbor(int userId, int itemId) const {
    const UserVec& uv = mTrainData->GetUserVector(userId);
    int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
    RunningMaxK<float> neighbors(neighborSize);
    for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
        float sim = mModel->Get(ir->ItemId(), itemId);
        neighbors.Add(sim);
    }
    float score = 0.0f;
    for (const float *ni = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
            ni != end; ++ni) {
        score += *ni;
    }
    return score;
}

float ItemBasedPredict::ComputeItemSimilarity(int itemId1, int itemId2) const {
    return mModel->Get(itemId1, itemId2);
}

void ItemBasedPredict::Cleanup() {
    Log::I("recsys", "ItemBasedPredict::Cleanup()");
    delete mPredictOption;
    delete mParameter;
    delete mTrainData;
    delete mTrainDataTrait;
    delete mModel;
}

} //~ namespace longan
