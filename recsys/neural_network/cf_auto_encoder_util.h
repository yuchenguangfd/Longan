/*
 * cf_auto_encoder_util.h
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_UTIL_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_UTIL_H

#include <json/json.h>

namespace longan {

namespace CFAE {

class Architecture {
public:
    Architecture(const Json::Value& arch);
    int NumHiddenUnit() const { return mNumHiddenUnit;}
private:
    int mNumHiddenUnit;
};

class Parameter {
public:
    Parameter(const Json::Value& para);
    int PossibleRatings() const { return mPossibleRatings; }
private:
    int mPossibleRatings;
};

class TrainOption {
public:
    TrainOption(const Json::Value& option);
    bool IsRandomInit() const { return mIsRandomInit; }
    int Iterations() const { return mIterations; }
    double Lambda() const { return mLambda; }
    double LearningRate() const { return mLearningRate; }
    bool Accelerate() const { return mAccelerate; }
    int NumThread() const { return mNumThread; }
    int IterationCheckStep() const { return mIterationCheckStep; }
private:
    bool mIsRandomInit;
    int mIterations;
    double mLambda;
    double mLearningRate;
    bool mAccelerate;
    int mNumThread;
    int mIterationCheckStep;
};

} //~ namespace CFAE

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_UTIL_H */
