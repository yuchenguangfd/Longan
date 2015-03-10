/*
 * cf_auto_encoder_util.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_util.h"
#include "common/error.h"
#include <cassert>

namespace longan {

namespace CFAE {

Parameter::Parameter(const Json::Value& param) {
    if (param["ratingType"].asString() == "numerical") {
        mRatingType = RatingType_Numerical;
    } else if (param["ratingType"].asString() == "binary") {
        mRatingType = RatingType_Binary;
    } else {
        throw LonganConfigError();
    }
    if (param["codeType"].asString() == "user") {
        mCodeType = CodeType_User;
    } else if (param["codeType"].asString() == "item") {
        mCodeType = CodeType_Item;
    } else {
        throw LonganConfigError();
    }
    int numLayer = param["architecture"].size();
    assert(numLayer > 0);
    mArchitecture.resize(numLayer);
    for (int i = 0; i < numLayer; ++i) {
        mArchitecture[i] = param["architecture"][i].asInt();
        assert(mArchitecture[i] > 0);
    }
}

TrainOption::TrainOption(const Json::Value& option) {
    mRandomInit = option["randomInit"].asBool();
    mIterations = option["iterations"].asInt();
    assert(mIterations > 0);
    mLambda = option["lambda"].asDouble();
    assert(mLambda >= 0.0);
    mLearningRate = option["learningRate"].asDouble();
    assert(mLearningRate > 0.0);
    mAccelerate = option["accelerate"].asBool();
    if (mAccelerate) {
        mNumThread = option["numThread"].asInt();
        assert(mNumThread >= 1);
    } else {
        mNumThread = 1;
    }
    mMonitorIteration = option["monitorIteration"].asBool();
    if (mMonitorIteration) {
        mMonitorIterationStep = option["monitorIterationStep"].asInt();
        if (mMonitorIterationStep <= 0) mMonitorIterationStep = 1;
    } else {
        mMonitorIterationStep = 0;
    }
    mMonitorProgress = option["monitorProgress"].asBool();
}

PredictOption::PredictOption(const Json::Value& option) {
    if (option["predictRatingMethod"].asString() == "reconstruct") {
        mPredictRatingMethod = PredictRatingMethod_Reconstruct;
    } else if (option["predictRatingMethod"].asString() == "codeItemNeighbor") {
        mPredictRatingMethod = PredictRatingMethod_CodeItemNeighbor;
    } else if (option["predictRatingMethod"].asString() == "codeUserNeighbor") {
        mPredictRatingMethod = PredictRatingMethod_CodeUserNeighbor;
    } else {
        throw LonganConfigError();
    }
    if (option["predictRankingMethod"].asString() == "predictRating") {
        mPredictRankingMethod = PredictRankingMethod_PredictRating;
    }else if (option["predictRatingMethod"].asString() == "codeItemNeighbor") {
        mPredictRankingMethod = PredictRankingMethod_CodeItemNeighbor;
    } else if (option["predictRatingMethod"].asString() == "codeUserNeighbor") {
        mPredictRankingMethod = PredictRankingMethod_CodeUserNeighbor;
    } else {
        throw LonganConfigError();
    }
    if (option["codeDistanceType"].asString() == "normL1") {
        mCodeDistanceType = CodeDistanceType_NormL1;
    } else if (option["codeDistanceType"].asString() == "normL2") {
        mCodeDistanceType = CodeDistanceType_NormL2;
    } else if (option["codeDistanceType"].asString() == "cosine") {
        mCodeDistanceType = CodeDistanceType_Cosine;
    } else if (option["codeDistanceType"].asString() == "correlation") {
        mCodeDistanceType = CodeDistanceType_Correlation;
    } else if (option["codeDistanceType"].asString() == "hamming") {
        mCodeDistanceType = CodeDistanceType_Hamming;
    } else {
        throw LonganConfigError();
    }
    mNeighborSize = option["neighborSize"].asInt();
}

} //~ namespace CFAE

} //~ namespace longan

