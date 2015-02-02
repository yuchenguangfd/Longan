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
#include <omp.h>

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
    std::vector<int> sampleOrder(mDatamodel->NumSample());
    ArrayHelper::FillRange(sampleOrder.data(), sampleOrder.size());
    Stopwatch sw;
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(sampleOrder.data(), sampleOrder.size());
        for (int i = 0; i < mDatamodel->NumSample(); ++i) {
            mActivations[0] = mDatamodel->Feature(sampleOrder[i]);
            mTarget = mDatamodel->Target(sampleOrder[i]);
            Forward();
            Backward();
            ComputeGradient();
            AdjustNetwork();
        }
        if (mTrainOption->IterationCheckStep() > 0 && iter % mTrainOption->IterationCheckStep() == 0) {
            ComputeTotalCost();
            Log::I("ml", "iter %d, loss=%lf, time=%.2lfs", iter, mTotalCost, sw.Toc());
            sw.Tic();
        }
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
    for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
        int rows = mNetwork->mArchitecture.NumLayerUnit(i+1);
        int cols = mNetwork->mArchitecture.NumLayerUnit(i);
        mDeltas[i] = std::move(Vector64F(rows));
        mWeightGradients[i] = std::move(Matrix64F(rows, cols));
        mBiasGradients[i] = std::move(Vector64F(rows));
    }
}

void BpNetworkComputationSimple::Forward() {
    for (int i = 1; i < mNetwork->mNumLayer; ++i) {
        mActivations[i] = Sigmoid(mNetwork->mWeights[i-1]*mActivations[i-1] + mNetwork->mBiases[i-1]);
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
        mNetwork->mWeights[i] -= learningRate*(mWeightGradients[i] + lambda*mNetwork->mWeights[i]);
        mNetwork->mBiases[i] -= learningRate*mBiasGradients[i];
    }
}

void BpNetworkComputationSimple::ComputeTotalCost() {
    double dataCost = 0.0;
    for (int i = 0; i < mDatamodel->NumSample(); ++i) {
        mActivations[0] = mDatamodel->Feature(i);
        Forward();
        dataCost += NormL2Sqr(mActivations[mNetwork->mNumLayer-1] - mDatamodel->Target(i));
    }
    dataCost /= 2.0 * mDatamodel->NumSample();
    double regCost = 0.0;
    for (const Matrix64F& weight : mNetwork->mWeights) {
        regCost +=  NormFSqr(weight);
    }
    regCost *= 0.5 * mTrainOption->Lambda();
    mTotalCost = dataCost + regCost;
}

//============================================================

void BpNetworkComputationOpenMP::Train(BpNetwork* network,
        const BpNetworkTrainOption* trainOption, const SupervisedDatamodel* datamodel) {
    mNetwork = network;
    mTrainOption = trainOption;
    mDatamodel = datamodel;
    omp_set_num_threads(mTrainOption->NumThread());
    if (mTrainOption->IsRandomInit()) {
        RandomInit();
    }
    InitResource();
    std::vector<int> sampleOrder;
    sampleOrder.resize(mDatamodel->NumSample());
    ArrayHelper::FillRange(sampleOrder.data(), sampleOrder.size());
    Stopwatch stopwatch;
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(sampleOrder.data(), sampleOrder.size());
        for (int i = 0; i < mDatamodel->NumSample(); ++i) {
            mCurrentSampleId = sampleOrder[i];
            mActivations[0] = mDatamodel->Feature(mCurrentSampleId);
            mTarget = mDatamodel->Target(mCurrentSampleId);
            Forward();
            Backward();
            ComputeGradient();
            AdjustNetwork();
        }
        if (mTrainOption->IterationCheckStep() > 0 && iter % mTrainOption->IterationCheckStep() == 0) {
            ComputeTotalLoss();
            Log::I("ml", "[OpenMP]iter %d, data loss=%lf, reg loss=%lf, "
                   "total loss=%lf, time=%.2lfs", iter, mDataLoss, mRegLoss, mTotalLoss,
                   stopwatch.Toc());
            stopwatch.Tic();
        }
    }
}

void BpNetworkComputationOpenMP::InitResource() {
    mTarget = std::move(Vector64F(mNetwork->mArchitecture.NumOutputLayerUnit()));
    mActivations.resize(mNetwork->mNumLayer);
    for (int i = 0; i < mNetwork->mNumLayer; ++i) {
        mActivations[i] = std::move(Vector64F(mNetwork->mArchitecture.NumLayerUnit(i)));
    }
    mDeltas.resize(mNetwork->mNumLayer - 1);
    mWeightGradients.resize(mNetwork->mNumLayer - 1);
    mBiasGradients.resize(mNetwork->mNumLayer - 1);
    for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
        int rows = mNetwork->mArchitecture.NumLayerUnit(i+1);
        int cols = mNetwork->mArchitecture.NumLayerUnit(i);
        mDeltas[i] = std::move(Vector64F(rows));
        mWeightGradients[i] = std::move(Matrix64F(rows, cols));
        mBiasGradients[i] = std::move(Vector64F(rows));
    }
    mSampleLosses.resize(mDatamodel->NumSample());
}

void BpNetworkComputationOpenMP::Forward() {
    int inputUnit, outputUnit;
    const double *inputAct, *weight, *bias;
    double *outputAct;
    for (int i = 1; i < mNetwork->mNumLayer; ++i) {
        inputUnit = mNetwork->mArchitecture.NumLayerUnit(i-1);
        outputUnit = mNetwork->mArchitecture.NumLayerUnit(i);
        inputAct = mActivations[i-1].Data();
        weight = mNetwork->mWeights[i-1].Data();
        bias = mNetwork->mBiases[i-1].Data();
        outputAct = mActivations[i].Data();
        ForwardOneLayer(inputUnit, outputUnit, inputAct, weight, bias, outputAct);
    }
}

void BpNetworkComputationOpenMP::ForwardOneLayer(int inputUnit, int outputUnit, const double *inputAct,
        const double *weight, const double *bias, double *outputAct) {
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < outputUnit; ++i) {
        double sum = bias[i];
        const double *rowData = weight+i*inputUnit;
        for (int j = 0; j < inputUnit; ++j) {
            sum += rowData[j] * inputAct[j];
        }
        outputAct[i] = Math::Sigmoid(sum);
    }
}

void BpNetworkComputationOpenMP::Backward() {
    int targetUnit, inputUnit, outputUnit;
    const double *inputAct, *targetAct, *inputDelta, *weight;
    double *outputDelta;

    targetUnit = mNetwork->mArchitecture.NumOutputLayerUnit();
    inputAct = mActivations[mNetwork->mNumLayer-1].Data();
    targetAct = mTarget.Data();
    outputDelta = mDeltas[mNetwork->mNumLayer-2].Data();
    BackwardOutputLayer(targetUnit, inputAct, targetAct, outputDelta);

    for (int i = mNetwork->mNumLayer - 3; i >= 0; --i) {
        inputUnit = mNetwork->mArchitecture.NumLayerUnit(i+1);
        outputUnit = mNetwork->mArchitecture.NumLayerUnit(i+2);
        weight = mNetwork->mWeights[i+1].Data();
        inputDelta = mDeltas[i+1].Data();
        inputAct = mActivations[i+1].Data();
        outputDelta = mDeltas[i].Data();
        BackwardOneLayer(inputUnit, outputUnit, weight, inputDelta, inputAct, outputDelta);
    }
}

void BpNetworkComputationOpenMP::BackwardOutputLayer(int targetUnit, const double *inputAct,
        const double *targetAct, double *outputDelta) {
    double sampleLoss = 0.0;
    for (int i = 0; i < targetUnit; ++i) {
        double d = inputAct[i] - targetAct[i];
        outputDelta[i] = (d) * inputAct[i] * (1.0 - inputAct[i]);
        sampleLoss += d*d;
    }
    mSampleLosses[mCurrentSampleId] = sampleLoss;
}

void BpNetworkComputationOpenMP::BackwardOneLayer(int inputUnit, int outputUnit, const double *weight,
        const double *inputDelta, const double *inputAct, double *outputDelta) {
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < inputUnit; ++i) {
        double sum = 0.0;
        for (int j = 0; j < outputUnit; ++j) {
            sum += weight[j*inputUnit+i] * inputDelta[j];
        }
        outputDelta[i] = sum * inputAct[i] * (1.0 - inputAct[i]);
    }
}

void BpNetworkComputationOpenMP::ComputeGradient() {
    int rows, cols;
    const double *delta, *act;
    double *weightGrad, *biasGrad;
    for (int i = mNetwork->mNumLayer - 2; i >= 0; --i) {
        rows = mDeltas[i].Size();
        cols = mActivations[i].Size();
        delta = mDeltas[i].Data();
        act = mActivations[i].Data();
        weightGrad = mWeightGradients[i].Data();
        biasGrad = mBiasGradients[i].Data();
        ComputeGradientOneLayer(rows, cols, delta, act, weightGrad, biasGrad);
    }
}

void BpNetworkComputationOpenMP::ComputeGradientOneLayer(int rows, int cols, const double *delta,
        const double *act, double *weightGrad, double *biasGrad) {
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            weightGrad[i*cols+j] = delta[i] * act[j];
        }
        biasGrad[i] = delta[i];
    }
}

void BpNetworkComputationOpenMP::AdjustNetwork() {
    int rows, cols;
    const double *weightGrad, *biasGrad;
    double *weight, *bias;
    for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
        rows = mNetwork->mWeights[i].Rows();
        cols = mNetwork->mWeights[i].Cols();
        weightGrad = mWeightGradients[i].Data();
        biasGrad =  mBiasGradients[i].Data();
        weight = mNetwork->mWeights[i].Data();
        bias = mNetwork->mBiases[i].Data();
        AdjustNetworkOneLayer(rows, cols, weightGrad, biasGrad, weight, bias);
    }
}

void BpNetworkComputationOpenMP::AdjustNetworkOneLayer(int rows, int cols, const double *weightGrad,
        const double *biasGrad, double *weight, double *bias) {
    double learningRate = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
#pragma omp parallel for schedule(dynamic)
    for (int i = rows-1; i >= 0; --i) {
        int rowBeginIdx = i*cols;
        for (int j = 0; j < cols; ++j) {
            int k = rowBeginIdx + j;
            weight[k] -= learningRate*(weightGrad[k] + lambda*weight[k]);
        }
        bias[i] -= learningRate*biasGrad[i];
    }
}

void BpNetworkComputationOpenMP::ComputeTotalLoss() {
    mDataLoss = 0.0;
    for (int i = 0; i < mDatamodel->NumSample(); ++i) {
        mDataLoss += mSampleLosses[i];
    }
    mDataLoss /= 2.0 * mDatamodel->NumSample();
    mRegLoss = 0.0;
    for (const Matrix64F& weight : mNetwork->mWeights) {
        mRegLoss +=  NormFSqr(weight);
    }
    mRegLoss *= 0.5 * mTrainOption->Lambda();
    mTotalLoss = mDataLoss + mRegLoss;
}

//============================================================

void BpNetworkComputationMT::Train(BpNetwork* network,
        const BpNetworkTrainOption* trainOption, const SupervisedDatamodel* datamodel) {
    mNetwork = network;
    mTrainOption = trainOption;
    mDatamodel = datamodel;
    if (mTrainOption->IsRandomInit()) {
        RandomInit();
    }
    InitResource();
    mThreadId = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mTrainOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void BpNetworkComputationMT::InitResource() {
    int numThread = mTrainOption->NumThread();
    mTarget.resize(numThread);
    mActivations.resize(numThread);
    mDeltas.resize(numThread);;
    mWeightGradients.resize(numThread);
    mBiasGradients.resize(numThread);
    for (int threadId = 0; threadId < mTrainOption->NumThread(); ++threadId) {
        mActivations[threadId].resize(mNetwork->mNumLayer);
        for (int i = 0; i < mNetwork->mNumLayer; ++i) {
            mActivations[threadId][i] = std::move(Vector64F(mNetwork->mArchitecture.NumLayerUnit(i)));
        }
        mDeltas[threadId].resize(mNetwork->mNumLayer - 1);
        mWeightGradients[threadId].resize(mNetwork->mNumLayer - 1);
        mBiasGradients[threadId].resize(mNetwork->mNumLayer - 1);
        for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
            int rows = mNetwork->mArchitecture.NumLayerUnit(i+1);
            int cols = mNetwork->mArchitecture.NumLayerUnit(i);
            mDeltas[threadId][i] = std::move(Vector64F(rows));
            mWeightGradients[threadId][i] = std::move(Matrix64F(rows, cols));
            mBiasGradients[threadId][i] = std::move(Vector64F(rows));
        }
    }
    mSampleLosses.resize(mDatamodel->NumSample());
}

void BpNetworkComputationMT::ProducerRun() {
    std::vector<int> sampleOrder(mDatamodel->NumSample());
    ArrayHelper::FillRange(sampleOrder.data(), sampleOrder.size());
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(sampleOrder.data(), sampleOrder.size());
        for (int i = 0; i < sampleOrder.size(); ++i) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->ProducerPutTask(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            Task task;
            task.type = TaskTypeCompute;
            task.sampleId = sampleOrder[i];
            currentBundle->push_back(task);
        }
        if (mTrainOption->IterationCheckStep() > 0 && iter % mTrainOption->IterationCheckStep() == 0) {
            Task task;
            task.type = TaskTypeMonitor;
            task.iter = iter;
            currentBundle->push_back(task);
        }
    }
    mScheduler->ProducerPutTask(currentBundle);
    mScheduler->ProducerDone();
}

void BpNetworkComputationMT::WorkerRun(int threadId) {
    assert(0 <= threadId && threadId < mTrainOption->NumThread());
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
              Task& task = currentBundle->at(i);
              if (task.type == TaskTypeMonitor) continue;
              ProcessTask(threadId, task.sampleId, &task.sampleLoss);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void BpNetworkComputationMT::ConsumerRun() {
    Stopwatch stopwatch;
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            if (task.type == TaskTypeMonitor) {
                ComputeTotalLoss();
                Log::I("ml", "[MT]iter %d, data loss=%lf, reg loss=%lf, "
                       "total loss=%lf, time=%.2lfs", task.iter, mDataLoss, mRegLoss, mTotalLoss,
                       stopwatch.Toc());
                stopwatch.Tic();
            } else {
                mSampleLosses[task.sampleId] = task.sampleLoss;
            }
        }
        delete currentBundle;
    }
    mScheduler->ConsumerDone();
}

void BpNetworkComputationMT::ProcessTask(int threadId, int sampleId, double *loss) {
    mActivations[threadId][0] = mDatamodel->Feature(sampleId);
    mTarget[threadId] = mDatamodel->Target(sampleId);
    Forward(threadId);
    Backward(threadId, loss);
    ComputeGradient(threadId);
    AdjustNetwork(threadId);
}

void BpNetworkComputationMT::Forward(int threadId) {
    int inputUnit, outputUnit;
    const double *inputAct, *weight, *bias;
    double *outputAct;
    for (int i = 1; i < mNetwork->mNumLayer; ++i) {
        inputUnit = mNetwork->mArchitecture.NumLayerUnit(i-1);
        outputUnit = mNetwork->mArchitecture.NumLayerUnit(i);
        inputAct = mActivations[threadId][i-1].Data();
        weight = mNetwork->mWeights[i-1].Data();
        bias = mNetwork->mBiases[i-1].Data();
        outputAct = mActivations[threadId][i].Data();
        ForwardOneLayer(inputUnit, outputUnit, inputAct, weight, bias, outputAct);
    }
}

void BpNetworkComputationMT::ForwardOneLayer(int inputUnit, int outputUnit, const double *inputAct,
        const double *weight, const double *bias, double *outputAct) {
    for (int i = 0; i < outputUnit; ++i) {
        double sum = bias[i];
        const double *rowData = weight+i*inputUnit;
        for (int j = 0; j < inputUnit; ++j) {
            sum += rowData[j] * inputAct[j];
        }
        outputAct[i] = Math::Sigmoid(sum);
    }
}

void BpNetworkComputationMT::Backward(int threadId, double *loss) {
    int targetUnit, inputUnit, outputUnit;
    const double *inputAct, *targetAct, *inputDelta, *weight;
    double *outputDelta;
    targetUnit = mNetwork->mArchitecture.NumOutputLayerUnit();
    inputAct = mActivations[threadId][mNetwork->mNumLayer-1].Data();
    targetAct = mTarget[threadId].Data();
    outputDelta = mDeltas[threadId][mNetwork->mNumLayer-2].Data();
    BackwardOutputLayer(targetUnit, inputAct, targetAct, outputDelta, loss);
    for (int i = mNetwork->mNumLayer - 3; i >= 0; --i) {
        inputUnit = mNetwork->mArchitecture.NumLayerUnit(i+1);
        outputUnit = mNetwork->mArchitecture.NumLayerUnit(i+2);
        weight = mNetwork->mWeights[i+1].Data();
        inputDelta = mDeltas[threadId][i+1].Data();
        inputAct = mActivations[threadId][i+1].Data();
        outputDelta = mDeltas[threadId][i].Data();
        BackwardOneLayer(inputUnit, outputUnit, weight, inputDelta, inputAct, outputDelta);
    }
}

void BpNetworkComputationMT::BackwardOutputLayer(int targetUnit, const double *inputAct,
        const double *targetAct, double *outputDelta, double *loss) {
    double error = 0.0;
    for (int i = 0; i < targetUnit; ++i) {
        double d = inputAct[i] - targetAct[i];
        outputDelta[i] = (d) * inputAct[i] * (1.0 - inputAct[i]);
        error += d * d;
    }
    *loss = error;
}

void BpNetworkComputationMT::BackwardOneLayer(int inputUnit, int outputUnit, const double *weight,
        const double *inputDelta, const double *inputAct, double *outputDelta) {
    for (int i = 0; i < inputUnit; ++i) {
        double sum = 0.0;
        for (int j = 0; j < outputUnit; ++j) {
            sum += weight[j*inputUnit+i] * inputDelta[j];
        }
        outputDelta[i] = sum * inputAct[i] * (1.0 - inputAct[i]);
    }
}

void BpNetworkComputationMT::ComputeGradient(int threadId) {
    int rows, cols;
    const double *delta, *act;
    double *weightGrad, *biasGrad;
    for (int i = mNetwork->mNumLayer - 2; i >= 0; --i) {
        rows = mDeltas[threadId][i].Size();
        cols = mActivations[threadId][i].Size();
        delta = mDeltas[threadId][i].Data();
        act = mActivations[threadId][i].Data();
        weightGrad = mWeightGradients[threadId][i].Data();
        biasGrad = mBiasGradients[threadId][i].Data();
        ComputeGradientOneLayer(rows, cols, delta, act, weightGrad, biasGrad);
    }
}

void BpNetworkComputationMT::ComputeGradientOneLayer(int rows, int cols, const double *delta,
        const double *act, double *weightGrad, double *biasGrad) {
    for (int i = 0; i < rows; ++i) {
        double *rowData = weightGrad+i*cols;
        for (int j = 0; j < cols; ++j) {
            rowData[j] = delta[i] * act[j];
        }
        biasGrad[i] = delta[i];
    }
}

void BpNetworkComputationMT::AdjustNetwork(int threadId) {
    int rows, cols;
    const double *weightGrad, *biasGrad;
    double *weight, *bias;
    std::vector<int> order(mNetwork->mNumLayer - 1);
    for (int i = 0; i < mNetwork->mNumLayer - 1; ++i) {
        rows = mNetwork->mWeights[i].Rows();
        cols = mNetwork->mWeights[i].Cols();
        weightGrad = mWeightGradients[threadId][i].Data();
        biasGrad =  mBiasGradients[threadId][i].Data();
        weight = mNetwork->mWeights[i].Data();
        bias = mNetwork->mBiases[i].Data();
        AdjustNetworkOneLayer(rows, cols, weightGrad, biasGrad, weight, bias);
    }
}

void BpNetworkComputationMT::AdjustNetworkOneLayer(int rows, int cols, const double *weightGrad,
        const double *biasGrad, double *weight, double *bias) {
    double learningRate = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
    for (int i = 0; i < rows; ++i) {
        int rowBeginIdx = i*cols;
        for (int j = 0; j < cols; ++j) {
            int k = rowBeginIdx + j;
            weight[k] -= learningRate*(weightGrad[k]+lambda*weight[k]);
        }
        bias[i] -= learningRate*biasGrad[i];
    }
}

void BpNetworkComputationMT::ComputeTotalLoss() {
    mDataLoss = 0.0;
    for (int i = 0; i < mDatamodel->NumSample(); ++i) {
        mDataLoss += mSampleLosses[i];
    }
    mDataLoss /= 2.0 * mDatamodel->NumSample();
    mRegLoss = 0.0;
    for (const Matrix64F& weight : mNetwork->mWeights) {
        mRegLoss += NormFSqr(weight);
    }
    mRegLoss *= 0.5 * mTrainOption->Lambda();
    mTotalLoss = mDataLoss + mRegLoss;
}

} //~ namespace longan
