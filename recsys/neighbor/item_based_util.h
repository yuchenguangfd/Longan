/*
 * item_based_util.h
 * Created on: Mar 1, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_UTIL_H
#define RECSYS_NEIGHBOR_ITEM_BASED_UTIL_H

#include <json/json.h>

namespace longan {

namespace ItemBased {

class Parameter {
public:
    enum SimType {
        SimTypeCosine,
        SimTypeAdjustedCosine,
        SimTypeCorrelation
    };
    Parameter(const Json::Value& parameter);
    int SimType() const { return mSimType; }
private:
    int mSimType;
};

class TrainOption {
public:
    TrainOption(const Json::Value& option);
    bool Accelerate() const { return mAccelerate; }
    int NumThread() const { return mNumThread; }
    bool MonitorProgress() const { return mMonitorProgress; }
private:
    bool mAccelerate;
    int mNumThread;
    bool mMonitorProgress;
};

class PredictOption {
public:
    PredictOption(const Json::Value& option);
    int NeighborSize() const { return mNeighborSize; }
private:
    int mNeighborSize;
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_UTIL_H */
