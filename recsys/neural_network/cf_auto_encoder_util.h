/*
 * cf_auto_encoder_util.h
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_UTIL_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_UTIL_H

#include "common/math/vector.h"
#include "common/lang/types.h"
#include <json/json.h>

namespace longan {

namespace CFAE {

class Parameter {
public:
    enum RatingType {
        RatingType_Numerical,
        RatingType_Binary
    };
    enum CodeType {
        CodeType_User,
        CodeType_Item
    };
    Parameter(const Json::Value& param);
    int RatingType() const { return mRatingType; }
    int CodeType() const { return mCodeType; }
    int NumLayer() const { return mArchitecture.size(); }
    int NumLayerUnit(int layer) const { return mArchitecture[layer]; }
private:
    int mRatingType;
    int mCodeType;
    std::vector<int> mArchitecture;
};

class TrainOption {
public:
    TrainOption(const Json::Value& option);
    bool RandomInit() const { return mRandomInit; }
    int Iterations() const { return mIterations; }
    double Lambda() const { return mLambda; }
    double LearningRate() const { return mLearningRate; }
    bool Accelerate() const { return mAccelerate; }
    int NumThread() const { return mNumThread; }
    bool MonitorIteration() const { return mMonitorIteration; }
    int MonitorIterationStep() const { return mMonitorIterationStep; }
    bool MonitorProgress() const { return mMonitorProgress; }
private:
    bool mRandomInit;
    int mIterations;
    double mLambda;
    double mLearningRate;
    bool mAccelerate;
    int mNumThread;
    bool mMonitorIteration;
    int mMonitorIterationStep;
    bool mMonitorProgress;
};

class PredictOption {
public:
    enum PredictRatingMethod {
        PredictRatingMethod_Reconstruct,
        PredictRatingMethod_CodeItemNeighbor,
        PredictRatingMethod_CodeUserNeighbor
    };
    enum PredictRankingMethod {
        PredictRankingMethod_PredictRating,
        PredictRankingMethod_CodeItemNeighbor,
        PredictRankingMethod_CodeUserNeighbor
    };
    enum CodeDistanceType {
        CodeDistanceType_NormL1,
        CodeDistanceType_NormL2,
        CodeDistanceType_Cosine,
        CodeDistanceType_Correlation,
        CodeDistanceType_Hamming,
    };
    PredictOption(const Json::Value& option);
    int PredictRatingMethod() const { return mPredictRatingMethod; }
    int PredictRankingMethod() const { return mPredictRankingMethod; }
    int CodeDistanceType() const { return mCodeDistanceType; }
    int NeighborSize() const { return mNeighborSize; }
private:
    int mPredictRatingMethod;
    int mPredictRankingMethod;
    int mCodeDistanceType;
    int mNeighborSize;
};

class BinaryCode {
public:
    BinaryCode(const Vector64F& vec) {
        mSize = (vec.Size() % 32 == 0) ? vec.Size() / 32 : vec.Size() / 32 + 1;
        assert(mSize <= MAX_SIZE);
        std::fill(mValues, mValues + mSize, 0);
        for (int i = 0; i < vec.Size(); ++i) {
            int j = i / 32;
            int offset = i % 32;
            if (Math::Round(vec[i]) == 1) {
                mValues[j] = (mValues[j] | (1 << offset));
            }
        }
    }
    friend int HammingDistance(const BinaryCode& code1, const BinaryCode& code2);
private:
    static const int MAX_SIZE = 16;
    uint32 mValues[MAX_SIZE];
    uint8 mSize;
};

inline int BitCount(uint32 x) {
    uint32 mask = 0x11111111;
    uint32 s = x & mask;
    s += (x>>1) & mask;
    s += (x>>2) & mask;
    s += (x>>3) & mask;
    s = (s >> 16) + s;
    mask = 0xF0F;
    s = (s & mask) + ((s >> 4) & mask);
    s = (s + (s>>8)) & 0x3F;
    return s;
}

inline int HammingDistance(const BinaryCode& code1, const BinaryCode& code2) {
    assert(code1.mSize == code2.mSize);
    int dist = 0;
    for (int i = 0; i < code1.mSize; ++i) {
        dist += BitCount(code1.mValues[i] ^ code2.mValues[i]);
    }
    return dist;
}

} //~ namespace CFAE

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_UTIL_H */
