/*
 * item_based_util.cpp
 * Created on: Mar 1, 2015
 * Author: chenguangyu
 */

#include "item_based_util.h"
#include "common/system/system_info.h"

namespace longan {

namespace ItemBased {

Parameter::Parameter(const Json::Value& parameter) {
    if (parameter["simType"].asString() == "cosine") {
        mSimType = SimTypeCosine;
    } else if (parameter["simType"].asString() == "adjustedCosine") {
        mSimType = SimTypeAdjustedCosine;
    } else if (parameter["simType"].asString() == "correlation") {
        mSimType = SimTypeCorrelation;
    } else {
        mSimType = SimTypeCosine;
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
