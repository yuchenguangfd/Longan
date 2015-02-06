/*
 * cf_auto_encoder_util.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_util.h"
#include <cassert>

namespace longan {

namespace CFAE {

Architecture::Architecture(const Json::Value& arch) {
    mNumHiddenUnit = arch["numHiddenUnit"].asInt();
    assert(mNumHiddenUnit > 0);
}

Parameter::Parameter(const Json::Value& para) {
    mPossibleRatings = para["possibleRatings"].asInt();
    assert(mPossibleRatings > 0);
}

TrainOption::TrainOption(const Json::Value& option) {
    mIsRandomInit = option["isRandomInit"].asBool();
    mIterations = option["iterations"].asInt();
    assert(mIterations >= 0);
    mLambda = option["lambda"].asDouble();
    mLearningRate = option["learningRate"].asDouble();
    assert(mLearningRate > 0.0);
    mAccelerate = option["accelerate"].asBool();
    if (mAccelerate) {
        mNumThread = option["numThread"].asInt();
        assert(mNumThread >= 1);
    } else {
        mNumThread = 1;
    }
    mIterationCheckStep = option["iterationCheckStep"].asInt();
}

} //~ namespace CFAE

} //~ namespace longan
