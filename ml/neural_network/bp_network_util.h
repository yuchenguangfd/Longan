/*
 * bp_network_util.h
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#ifndef ML_NEURAL_NETWORK_BP_NETWORK_UTIL_H
#define ML_NEURAL_NETWORK_BP_NETWORK_UTIL_H

#include <json/json.h>

namespace longan {

class BpNetworkArchitecture {
public:
    BpNetworkArchitecture(const Json::Value& arch);
    int NumLayer() const { return mNumLayerUnits.size(); }
    int NumLayerUnit(int layer) const { return mNumLayerUnits[layer]; }
    int NumInputLayerUnit() const { return *mNumLayerUnits.begin(); }
    int NumOutputLayerUnit() const { return *mNumLayerUnits.rbegin();}
private:
    std::vector<int> mNumLayerUnits;
};

class BpNetworkTrainOption {
public:
    BpNetworkTrainOption(const Json::Value& option);
    bool IsRandomInit() const { return mIsRandomInit; }
    int Iterations() const { return mIterations; }
    double Lambda() const { return mLambda; }
    double LearningRate() const { return mLearningRate; }
    bool Accelerate() const { return mAccelerate; }
    bool UseOpenMP() const { return mUseOpenMP; }
    int NumThread() const { return mNumThread; }
    int IterationCheckStep() const { return mIterationCheckStep; }
private:
    bool mIsRandomInit;
    int mIterations;
    double mLambda;
    double mLearningRate;
    bool mAccelerate;
    bool mUseOpenMP;
    int mNumThread;
    int mIterationCheckStep;
};

} //~ namespace longan

#endif /* ML_NEURAL_NETWORK_BP_NETWORK_UTIL_H */
