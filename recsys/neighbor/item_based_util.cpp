/*
 * item_based_util.cpp
 * Created on: Mar 1, 2015
 * Author: chenguangyu
 */

#include "item_based_util.h"
#include "common/system/system_info.h"
#include "common/error.h"
#include <cassert>

namespace longan {

namespace ItemBased {

Parameter::Parameter(const Json::Value& parameter) {
    if (parameter["ratingType"].asString() == "numerical") {
        mRatingType = RatingTypeNumerical;
    } else if (parameter["ratingType"].asString() == "binary") {
        mRatingType = RatingTypeBinary;
    } else {
        throw LonganConfigError();
    }
    if (parameter["simType"].asString() == "cosine") {
        mSimType = SimTypeCosine;
        assert(mRatingType == RatingTypeNumerical);
    } else if (parameter["simType"].asString() == "adjustedCosine") {
        mSimType = SimTypeAdjustedCosine;
        assert(mRatingType == RatingTypeNumerical);
    } else if (parameter["simType"].asString() == "correlation") {
        mSimType = SimTypeCorrelation;
        assert(mRatingType == RatingTypeNumerical);
    } else if (parameter["simType"].asString() == "binaryCosine") {
        mSimType = SimTypeBinaryCosine;
        assert(mRatingType == RatingTypeBinary);
    } else if (parameter["simType"].asString() == "binaryJaccard") {
        mSimType = SimTypeBinaryJaccard;
        assert(mRatingType == RatingTypeBinary);
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
    mNeighborSize = option["neighborSize"].asInt();
}

}  //~ namespace ItemBased

} //~ namespace longan
