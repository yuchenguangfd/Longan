/*
 * item_based_predict.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/util/running_statistic.h"
#include <algorithm>
#include <cassert>

namespace longan {

void ItemBasedPredict::Init() {
    Log::I("recsys", "ItemBasedPredict::Init()");
    LoadConfig();
    CreatePredictOption();
    CreateParameter();
    LoadTrainData();
    AdjustRating();
    LoadModel();
    InitCachedTopNItems();
}

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
    if (mParameter->RatingType() == ItemBased::Parameter::RatingTypeNumerical) {
        mTrainDataTrait = new RatingTrait();
        mTrainDataTrait->Init(rlist);
    }
}

void ItemBasedPredict::AdjustRating() {
    if (mParameter->RatingType() == ItemBased::Parameter::RatingTypeNumerical) {
        Log::I("recsys", "ItemBasedPredict::AdjustRating()");
        if (mParameter->SimType() == ItemBased::Parameter::SimTypeAdjustedCosine) {
            AdjustRatingByMinusUserAverage(*mTrainDataTrait, mTrainData);
        } else if (mParameter->SimType() == ItemBased::Parameter::SimTypeCorrelation) {
            AdjustRatingByMinusItemAverage(*mTrainDataTrait, mTrainData);
        }
    }
}

void ItemBasedPredict::LoadModel() {
    Log::I("recsys", "ItemBasedPredict::LoadModel()");
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel = new ItemBased::ModelPredict();
    mModel->Load(mModelFilepath);
}

void ItemBasedPredict::InitCachedTopNItems() {
    Log::I("recsys", "ItemBasedPredict::InitCachedTopNItems()");
    mCachedTopNItems.resize(mTrainData->NumUser());
}

float ItemBasedPredict::PredictRating(int userId, int itemId) const {
	assert(userId >= 0 && userId < mTrainData->NumUser());
	assert(itemId >= 0 && itemId < mTrainData->NumItem());
    if (mParameter->RatingType() == ItemBased::Parameter::RatingTypeNumerical) {
        if (mPredictOption->NeighborSize() <= 0) {
            return PredictRatingByAllNeighbor(userId, itemId);
        } else {
            return PredictRatingByFixedSizeNeighbor(userId, itemId);
        }
    } else if (mParameter->RatingType() == ItemBased::Parameter::RatingTypeBinary) {
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
        float sim = mModel->GetSimilarity(ir->ItemId(), itemId);
        numerator += sim * ir->Rating();
        denominator += Math::Abs(sim);
    }
    double predRating = numerator / (denominator + Double::EPS);
    if (mParameter->SimType() == ItemBased::Parameter::SimTypeAdjustedCosine) {
        predRating += mTrainDataTrait->UserAverage(userId);
    } else if (mParameter->SimType() == ItemBased::Parameter::SimTypeCorrelation) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    }
    return (float)predRating;
}

float ItemBasedPredict::PredictRatingByFixedSizeNeighbor(int userId, int itemId) const {
    const UserVec& uv = mTrainData->GetUserVector(userId);
    int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
    RunningMaxK<ItemBased::NeighborItem> neighbors(neighborSize);
    for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
        float sim = mModel->GetSimilarity(ir->ItemId(), itemId);
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
    if (mParameter->SimType() == ItemBased::Parameter::SimTypeAdjustedCosine) {
        predRating += mTrainDataTrait->UserAverage(userId);
    } else if (mParameter->SimType() == ItemBased::Parameter::SimTypeCorrelation) {
        predRating += mTrainDataTrait->ItemAverage(itemId);
    }
    return (float)predRating;
}

ItemIdList ItemBasedPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(listSize > 0);
    if (listSize <= mCachedTopNItems[userId].size()) {
        return PredictTopNItemFromCache(userId, listSize);
    }
    int numItem = mTrainData->NumItem();
    const UserVec& uv = mTrainData->GetUserVector(userId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    std::vector<ItemRating> scores;
    scores.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size; ++i) {
        begin = end + 1;
        end = data[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            scores.push_back(ItemRating(iid, PremdictTopNItemComputeScore(userId, iid)));
        }
    }
    begin = end + 1;
    end = numItem;
    for (int iid = begin; iid < end; ++iid) {
        scores.push_back(ItemRating(iid, PremdictTopNItemComputeScore(userId, iid)));
    }
    std::sort(scores.begin(), scores.end(),
        [](const ItemRating& lhs, const ItemRating& rhs)->bool {
            return lhs.Rating() > rhs.Rating();
    });
    ItemIdList topNItem(listSize);
    if (listSize <= scores.size()) {
        for (int i = 0; i < listSize; ++i) {
            topNItem[i] = scores[i].ItemId();
        }
    } else {
        for (int i = 0; i < scores.size(); ++i) {
            topNItem[i] = scores[i].ItemId();
        }
    }
    mCachedTopNItems[userId] = topNItem;
    return std::move(topNItem);
}

ItemIdList ItemBasedPredict::PredictTopNItemFromCache(int userId, int listSize) const {
    ItemIdList topNItem(listSize);
    for (int i = 0; i < listSize; ++i) {
        topNItem[i] = mCachedTopNItems[userId][i];
    }
    return std::move(topNItem);
}

float ItemBasedPredict::PremdictTopNItemComputeScore(int userId, int itemId) const {
    if (mParameter->RatingType() == ItemBased::Parameter::RatingTypeNumerical) {
        return PredictRating(userId, itemId);
    } else if (mParameter->RatingType() == ItemBased::Parameter::RatingTypeBinary) {
        const UserVec& uv = mTrainData->GetUserVector(userId);
        float score = 0.0;
        if (mPredictOption->NeighborSize() <= 0) {
            for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
                float sim = mModel->GetSimilarity(ir->ItemId(), itemId);
                score += sim;
            }
        } else {
            int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
            RunningMaxK<float> neighbors(neighborSize);
            for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
                float sim = mModel->GetSimilarity(ir->ItemId(), itemId);
                neighbors.Add(sim);
            }
            for (const float *ni = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
                    ni != end; ++ni) {
                score += *ni;
            }
        }
        return score;
    } else {
        return 0.0f;
    }
}

float ItemBasedPredict::ComputeItemSimilarity(int itemId1, int itemId2) const {
    return mModel->GetSimilarity(itemId1, itemId2);
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
