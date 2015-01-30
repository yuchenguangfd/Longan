/*
 * bp_network_computation.cpp
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#include "bp_network_computation.h"
#include "bp_network.h"
#include "ml/base/math.h"
#include "common/util/random.h"
#include "common/util/array_helper.h"
#include "common/time/stopwatch.h"
#include "common/logging/logging.h"

namespace longan {

BpNetworkComputation::~BpNetworkComputation() { }

void BpNetworkComputation::RandomInit() {
    Random& rnd = Random::Instance();
    for (Matrix64F& weight : mNetwork->mWeights) {
        for (int i = 0; i < weight.Rows(); ++i) {
            for (int j = 0; j < weight.Cols(); ++j) {
                weight[i][j] = rnd.Uniform(-0.05, +0.05);
            }
        }
    }
    for (Vector64F& bias : mNetwork->mBiases) {
        for (int i = 0; i < bias.Size(); ++i) {
            bias[i] = rnd.Uniform(-0.05, +0.05);
        }
    }
}

void BpNetworkComputationSimple::Train(BpNetwork* network,
        const BpNetworkTrainOption* trainOption, const SupervisedDatamodel* datamodel) {
    mNetwork = network;
    mTrainOption = trainOption;
    mDatamodel = datamodel;
    if (mTrainOption->IsRandomInit()) {
        RandomInit();
    }
    InitResource();
    mSampleOrder.resize(mDatamodel->NumSample());
    ArrayHelper::FillRange(mSampleOrder.data(), mSampleOrder.size());
    Stopwatch sw;
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(mSampleOrder.data(), mSampleOrder.size());
        for (int i = 0; i < mDatamodel->NumSample(); ++i) {
            mActivations[0] = mDatamodel->Feature(mSampleOrder[i]);
            mTarget = mDatamodel->Target(mSampleOrder[i]);
            Forward();
            Backward();
            ComputeGradient();
            AdjustNetwork();
            if (i%1000==0) Log::Console("ml", "using sample %d", i);
        }
        ComputeTotalCost();
        Log::Console("ml", "finish iteration %d, cost func = %lf, time = %.2lfs",
                iter, mTotalCost,
                sw.Toc());
        sw.Tic();
    }
}

void BpNetworkComputationSimple::InitResource() {
    mActivations.resize(mNetwork->mNumLayer);
    for (int i = 0; i < mNetwork->mNumLayer; ++i) {
        mActivations[i] = std::move(Vector64F(mNetwork->mArchitecture.NumLayerUnit(i)));
    }
    mTarget = std::move(Vector64F(mNetwork->mArchitecture.NumOutputLayerUnit()));
    mDeltas.resize(mNetwork->mNumLayer - 1);
    mWeightGradients.resize(mNetwork->mNumLayer - 1);
    mBiasGradients.resize(mNetwork->mNumLayer - 1);
    mWeightAdjust.resize(mNetwork->mNumLayer - 1);
    mBiasAdjust.resize(mNetwork->mNumLayer - 1);
    for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
        int rows = mNetwork->mArchitecture.NumLayerUnit(i+1);
        int cols = mNetwork->mArchitecture.NumLayerUnit(i);
        mDeltas[i] = std::move(Vector64F(rows));
        mWeightGradients[i] = std::move(Matrix64F(rows, cols));
        mBiasGradients[i] = std::move(Vector64F(rows));
        mWeightAdjust[i] = std::move(Matrix64F(rows, cols));
        mBiasAdjust[i] = std::move(Vector64F(rows));
    }
}

void BpNetworkComputationSimple::Forward() {
    for (int i = 1; i < mNetwork->mNumLayer; ++i) {
        mActivations[i] = Sigmoid(
                mNetwork->mWeights[i-1]*mActivations[i-1]
              + mNetwork->mBiases[i-1]);
    }
}

void BpNetworkComputationSimple::Backward() {
    mDeltas[mNetwork->mNumLayer - 2] = MultiplyElementWise(
            mActivations[mNetwork->mNumLayer - 1] - mTarget,
            mActivations[mNetwork->mNumLayer - 1],
            1.0 - mActivations[mNetwork->mNumLayer - 1]);
    for (int i = mNetwork->mNumLayer - 3; i >= 0; --i) {
        mDeltas[i] = MultiplyElementWise(
            mNetwork->mWeights[i+1].Transpose() * mDeltas[i+1],
            mActivations[i+1],
            1.0 - mActivations[i+1]);
    }
}

void BpNetworkComputationSimple::ComputeGradient() {
    for (int i = mNetwork->mNumLayer - 2; i >= 0; --i) {
        mWeightGradients[i] = OutterProd(mDeltas[i], mActivations[i]);
        mBiasGradients[i] = mDeltas[i];
    }
}

void BpNetworkComputationSimple::AdjustNetwork() {
    double learningRate = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
    for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
        mWeightAdjust[i] = learningRate * (mWeightGradients[i] + lambda * mNetwork->mWeights[i]);
        mBiasAdjust[i] = learningRate * mBiasGradients[i];
        mNetwork->mWeights[i] -= mWeightAdjust[i];
        mNetwork->mBiases[i] -= mBiasAdjust[i];
    }
}

void BpNetworkComputationSimple::ComputeTotalCost() {
    double dataCost = 0.0;
    for (int i = 0; i < mDatamodel->NumSample(); ++i) {
        mActivations[0] = mDatamodel->Feature(i);
        Forward();
        dataCost += NormL2Sqr(mActivations[mNetwork->mNumLayer - 1] - mDatamodel->Target(i));
    }
    dataCost /= 2.0 * mDatamodel->NumSample();
    double regCost = 0.0;
    for (int i = 0; i < mNetwork->mWeights.size(); ++i) {
        regCost +=  NormFSqr(mNetwork->mWeights[i]);
    }
    regCost *= 0.5 * mTrainOption->Lambda();
    mTotalCost = dataCost + regCost;
}

} //~ namespace longan
