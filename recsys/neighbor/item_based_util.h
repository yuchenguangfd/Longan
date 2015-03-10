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
    enum RatingType {
        RatingType_Numerical,
        RatingType_Binary
    };
    enum SimType {
        SimType_Cosine,
        SimType_AdjustedCosine,
        SimType_Correlation,
        SimType_BinaryCosine,
        SimType_BinaryJaccard
    };
    Parameter(const Json::Value& param);
    int RatingType() const { return mRatingType; }
    int SimType() const { return mSimType; }
private:
    int mRatingType;
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
    enum PredictRankingMethod {
        PredictRankingMethod_PredictRating,
        PredictRankingMethod_NeighborSimilarity
    };
    PredictOption(const Json::Value& option);
    int PredictRankingMethod() const { return mPredictRankingMethod; }
    int NeighborSize() const { return mNeighborSize; }
private:
    int mPredictRankingMethod;
    int mNeighborSize;
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_UTIL_H */
