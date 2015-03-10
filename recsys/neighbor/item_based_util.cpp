/*
 * item_based_util.cpp
 * Created on: Mar 1, 2015
 * Author: chenguangyu
 */

#include "item_based_util.h"
#include "common/common.h"

namespace longan {

namespace ItemBased {

Parameter::Parameter(const Json::Value& param) {
    if (param["ratingType"].asString() == "numerical") {
        mRatingType = RatingType_Numerical;
    } else if (param["ratingType"].asString() == "binary") {
        mRatingType = RatingType_Binary;
    } else {
        throw LonganConfigError();
    }
    if (param["simType"].asString() == "cosine") {
        assert(mRatingType == RatingType_Numerical);
        mSimType = SimType_Cosine;
    } else if (param["simType"].asString() == "adjustedCosine") {
        assert(mRatingType == RatingType_Numerical);
        mSimType = SimType_AdjustedCosine;
    } else if (param["simType"].asString() == "correlation") {
        assert(mRatingType == RatingType_Numerical);
        mSimType = SimType_Correlation;
    } else if (param["simType"].asString() == "binaryCosine") {
        assert(mRatingType == RatingType_Binary);
        mSimType = SimType_BinaryCosine;
    } else if (param["simType"].asString() == "binaryJaccard") {
        assert(mRatingType == RatingType_Binary);
        mSimType = SimType_BinaryJaccard;
    } else {
        throw LonganConfigError();
    }
}

TrainOption::TrainOption(const Json::Value& option) {
    mAccelerate = option["accelerate"].asBool();
    if (mAccelerate) {
        mNumThread = option["numThread"].asInt();
        if (mNumThread == 0) {
            mNumThread = SystemInfo::GetNumCPUCore();
        }
    } else {
        mNumThread = 1;
    }
    mMonitorProgress = option["monitorProgress"].asBool();
}

PredictOption::PredictOption(const Json::Value& option) {
    if (option["predictRankingMethod"].asString() == "predictRating") {
        mPredictRankingMethod = PredictRankingMethod_PredictRating;
    } else if (option["predictRankingMethod"].asString() == "neighborSimilarity") {
        mPredictRankingMethod = PredictRankingMethod_NeighborSimilarity;
    } else {
        throw LonganConfigError();
    }
    mNeighborSize = option["neighborSize"].asInt();
}

}  //~ namespace ItemBased

} //~ namespace longan
