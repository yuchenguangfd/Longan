/*
 * bp_network_util.cpp
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#include "bp_network_util.h"
#include <cassert>

namespace longan {

BpNetworkArchitecture::BpNetworkArchitecture(const Json::Value& arch) {
    int numLayer = arch["numLayer"].asInt();
    assert(numLayer >= 2);
    const Json::Value& numLayerUnits = arch["numLayerUnits"];
    assert(numLayerUnits.size() == numLayer);
    mNumLayerUnits.resize(numLayer);
    for (int i = 0; i < numLayer; ++i) {
        mNumLayerUnits[i] = numLayerUnits[i].asInt();
        assert(mNumLayerUnits[i] > 0);
    }
}

BpNetworkTrainOption::BpNetworkTrainOption(const Json::Value& option) {
    mIsRandomInit = option["isRandomInit"].asBool();
    mIterations = option["iterations"].asInt();
    assert(mIterations >= 0);
    mLambda = option["lambda"].asDouble();
    mLearningRate = option["learningRate"].asDouble();
    assert(mLearningRate > 0.0);
    mAccelerate = option["accelerate"].asBool();
    if (mAccelerate) {
        mUseOpenMP = option["useOpenMP"].asBool();
        mNumThread = option["numThread"].asInt();
        assert(mNumThread >= 1);
    }
    mIterationCheckStep = option["iterationCheckStep"].asInt();
}

} //~ namespace longan
