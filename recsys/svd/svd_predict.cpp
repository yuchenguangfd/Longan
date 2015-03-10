/*
 * svd_predict.cpp
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#include "svd_predict.h"
#include "recsys/base/rating_list.h"
#include "common/common.h"

namespace longan {

void SVDPredict::CreatePredictOption() {
    Log::I("recsys", "SVDPredict::CreatePredictOption()");
    mPredictOption = new SVD::PredictOption(mConfig["predictOption"]);
}

void SVDPredict::CreateParameter() {
    Log::I("recsys", "SVDPredict::CreateParameter()");
    mParameter = new SVD::Parameter(mConfig["parameter"]);
}

void SVDPredict::LoadTrainData() {
    Log::I("recsys", "SVDPredict::LoadTrainData()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    BinaryInputStream bis(mRatingTrainFilepath);
    int numRating, numUser, numItem;
    float avgRating;
    bis >> numRating >> numUser >> numItem
        >> avgRating;
    RatingList rlist(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        rlist.Add(RatingRecord(uid, iid, rating));
    }
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
    if (mPredictOption->PredictRankingMethod() == SVD::PredictOption::PredictRankingMethod_LatentUserNeighbor) {
        mTrainDataItems = new RatingMatItems();
        mTrainDataItems->Init(rlist);
    }
}

void SVDPredict::LoadModel() {
    Log::I("recsys", "SVDPredict::LoadModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel = new SVD::Model(mParameter);
    mModel->Load(mModelFilepath);
}

float SVDPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mModel->NumUser());
    assert(itemId >= 0 && itemId < mModel->NumItem());
    const Vector32F& userFeature = mModel->UserFeature(userId);
    const Vector32F& itemFeature = mModel->ItemFeature(itemId);
    float predRating = InnerProd(userFeature, itemFeature);
    if (mParameter->LambdaUserBias() >= 0.0f) {
        predRating += mModel->UserBias(userId);
    }
    if (mParameter->LambdaItemBias() >= 0.0f) {
        predRating += mModel->ItemBias(itemId);
    }
    if (mParameter->UseRatingAverage()) {
        predRating += mModel->RatingAverage() ;
    }
    return mParameter->UseSigmoid() ? Math::Sigmoid(predRating) : predRating;
}

float SVDPredict::ComputeTopNItemScore(int userId, int itemId) const {
    if (mPredictOption->PredictRankingMethod() == SVD::PredictOption::PredictRankingMethod_PredictRating) {
        return PredictRating(userId, itemId);
    } else if (mPredictOption->PredictRankingMethod() == SVD::PredictOption::PredictRankingMethod_LatentItemNeighbor) {
        return ComputeTopNItemScoreByLatentItemNeighbor(userId, itemId);
    } else if (mPredictOption->PredictRankingMethod() == SVD::PredictOption::PredictRankingMethod_LatentUserNeighbor) {
        return ComputeTopNItemScoreByLatentUserNeighbor(userId, itemId);
    } else {
        return 0.0f;
    }
}

float SVDPredict::ComputeTopNItemScoreByLatentItemNeighbor(int userId, int itemId) const {
    if (mPredictOption->NeighborSize() <= 0) {
        const UserVec& uv = mTrainData->GetUserVector(userId);
        float score = 0.0f;
        for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
            float dist = ComputeLatentDistance(mModel->ItemFeature(ir->ItemId()), mModel->ItemFeature(itemId));
            score += dist;
        }
        score = -score;
        return score;
    } else {
        const UserVec& uv = mTrainData->GetUserVector(userId);
        int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
        RunningMinK<float> neighbors(neighborSize);
        for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
            float dist = ComputeLatentDistance(mModel->ItemFeature(ir->ItemId()), mModel->ItemFeature(itemId));
            neighbors.Add(dist);
        }
        float score = 0.0f;
        for (const float *ni = neighbors.CurrentMinKBegin(), *end = neighbors.CurrentMinKEnd();
                ni != end; ++ni) {
            score += *ni;
        }
        score = -score;
        return score;
    }
}

float SVDPredict::ComputeTopNItemScoreByLatentUserNeighbor(int userId, int itemId) const {
    if (mPredictOption->NeighborSize() <= 0) {
        const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
        float score = 0.0f;
        for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
            float dist = ComputeLatentDistance(mModel->UserFeature(ur->UserId()), mModel->UserFeature(userId));
            score += dist;
        }
        score = -score;
        return score;
    } else {
        const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
        int neighborSize = Math::Min(mPredictOption->NeighborSize(), iv.Size());
        RunningMinK<float> neighbors(neighborSize);
        for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
            float dist = ComputeLatentDistance(mModel->UserFeature(ur->UserId()), mModel->UserFeature(userId));
            neighbors.Add(dist);
        }
        float score = 0.0f;
        for (const float *nu = neighbors.CurrentMinKBegin(), *end = neighbors.CurrentMinKEnd();
                nu != end; ++nu) {
            score += *nu;
        }
        score = -score;
        return score;
    }
}

float SVDPredict::ComputeLatentDistance(const Vector32F& vec1, const Vector32F& vec2) const {
    switch (mPredictOption->LatentDistanceType()) {
        case SVD::PredictOption::LatentDistanceType_NormL1:
            return DistanceL1(vec1, vec2);
        case SVD::PredictOption::LatentDistanceType_NormL2:
            return DistanceL2(vec1, vec2);
        case SVD::PredictOption::LatentDistanceType_Cosine:
            return DistanceCosine(vec1, vec2);
        case SVD::PredictOption::LatentDistanceType_Correlation:
            return DistanceCorrelation(vec1, vec2);
        default:
            return 0.0f;
    }
}

void SVDPredict::Cleanup() {
    delete mPredictOption;
    delete mParameter;
    delete mTrainData;
    delete mTrainDataItems;
    delete mModel;
}

} //~ namespace longan
