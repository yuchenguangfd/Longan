/*
 * cf_auto_encoder_computation.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_computation.h"
#include "cf_auto_encoder.h"
#include "ml/base/math.h"
#include "common/util/random.h"
#include "common/util/array_helper.h"
#include "common/time/stopwatch.h"
#include "common/logging/logging.h"

namespace longan {

CFAutoEncoderComputation::~CFAutoEncoderComputation() { }

void CFAutoEncoderComputation::RandomInit() {
    Random& rnd = Random::Instance();
    Matrix64F& weight1 = mAutoEncoder->mWeight1;
    for (int i = 0; i < weight1.Rows(); ++i) {
        for (int j = 0; j < weight1.Cols(); ++j) {
            weight1[i][j] = rnd.Uniform(-0.05, +0.05);
        }
    }
    Vector64F& bias1 = mAutoEncoder->mBias1;
    for (int i = 0; i < bias1.Size(); ++i) {
        bias1[i] = rnd.Uniform(-0.05, +0.05);
    }
    Matrix64F& weight2 = mAutoEncoder->mWeight2;
    for (int i = 0; i < weight2.Rows(); ++i) {
        for (int j = 0; j < weight2.Cols(); ++j) {
            weight2[i][j] = rnd.Uniform(-0.05, +0.05);
        }
    }
    Vector64F& bias2 = mAutoEncoder->mBias2;
    for (int i = 0; i < bias1.Size(); ++i) {
        bias2[i] = rnd.Uniform(-0.05, +0.05);
    }
}

void CFAutoEncoderComputationST::Train(CFAutoEncoder *autoEncoder, const CFAE::TrainOption *trainOption,
        const RatingMatrixAsUsers<> *data) {
    mAutoEncoder = autoEncoder;
    mTrainOption = trainOption;
    mData = data;
    if (mTrainOption->IsRandomInit()) {
        RandomInit();
    }
    InitResource();
    std::vector<int> sampleOrder;
    sampleOrder.resize(mData->NumUser());
    ArrayHelper::FillRange(sampleOrder.data(), sampleOrder.size());
    Stopwatch stopwatch;
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(sampleOrder.data(), sampleOrder.size());
        for (int i = 0; i < mData->NumUser(); ++i) {
            if (i%1000 == 0)Log::Console("recsys", "sample %d/%d", i, mData->NumUser());
            mCurrentSampleId = sampleOrder[i];
            Forward();
            Backward();
            UpdateNetwork();
        }
        if (mTrainOption->IterationCheckStep() > 0 && iter % mTrainOption->IterationCheckStep() == 0) {
            ComputeTotalLoss();
            Log::I("recsys", "[ST]iter %d, dataLoss=%lf, regLoss=%lf, "
                   "totalLoss=%lf, time=%.2lfs", iter, mDataLoss, mRegLoss, mTotalLoss,
                   stopwatch.Toc());
            stopwatch.Tic();
        }
    }
}

void CFAutoEncoderComputationST::InitResource() {
    mActivation1 = std::move(Vector64F(mAutoEncoder->mNumHiddenUnit));
    mActivation2 = std::move(Vector64F(mAutoEncoder->mNumOutputUnit));
    mDelta1 = std::move(Vector64F(mAutoEncoder->mNumHiddenUnit));
    mDelta2 = std::move(Vector64F(mAutoEncoder->mNumOutputUnit));
    mSampleLosses.resize(mData->NumUser());
}

void CFAutoEncoderComputationST::Forward() {
    ForwardInputToHidden();
    ForwardHiddenToOutput();
}

void CFAutoEncoderComputationST::ForwardInputToHidden() {
    const auto& uv = mData->GetUserVector(mCurrentSampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    for (int i = 0; i < mAutoEncoder->mNumHiddenUnit; ++i) {
        double sum = mAutoEncoder->mBias1[i];
        for (int j = 0; j < size; ++j) {
            int jj = data[j].ItemId()*5 + static_cast<int>(data[j].Rating());
            sum += mAutoEncoder->mWeight1[i][jj];
        }
        mActivation1[i] = Math::Sigmoid(sum);
    }
}

void CFAutoEncoderComputationST::ForwardHiddenToOutput() {
    const auto& uv = mData->GetUserVector(mCurrentSampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    for (int ii = 0; ii < size; ++ii) {
        int itemId = data[ii].ItemId();
        for (int i = itemId * 5; i < itemId * 5 + 5; ++i) {
            double sum = mAutoEncoder->mBias2[i];
            for (int j = 0; j < mAutoEncoder->mNumHiddenUnit; ++j) {
                  sum += mAutoEncoder->mWeight2[i][j] * mActivation1[j];
            }
            mActivation2[i] = Math::Sigmoid(sum);
        }
    }
}

void CFAutoEncoderComputationST::Backward() {
    BackwardOutputToHidden();
    BackwardHiddenToInput();
}

void CFAutoEncoderComputationST::BackwardOutputToHidden() {
    double sampleLoss = 0.0;
    const auto& uv = mData->GetUserVector(mCurrentSampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    for (int ii = 0; ii < size; ++ii) {
        int itemId = data[ii].ItemId();
        int rating = static_cast<int>(data[ii].Rating());
        for (int i = itemId * 5, k = 0; k < 5; ++i, ++k) {
            double d = mActivation2[i] - ((k==rating)?1.0:0.0);
            mDelta2[i] = (d) * mActivation2[i] * (1.0 - mActivation2[i]);
            sampleLoss += d*d;
        }
    }
    mSampleLosses[mCurrentSampleId] = sampleLoss;
}

void CFAutoEncoderComputationST::BackwardHiddenToInput() {
    const auto& uv = mData->GetUserVector(mCurrentSampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    for (int i = 0; i < mAutoEncoder->mNumHiddenUnit; ++i) {
        double sum = 0.0;
        for (int jj = 0; jj < size; ++jj) {
            int itemId = data[jj].ItemId();
            for (int j = itemId * 5; j < itemId*5+5; ++j) {
                sum += mAutoEncoder->mWeight2[j][i] * mDelta2[j];
            }
        }
        mDelta1[i] = sum * mActivation1[i] * (1.0 - mActivation1[i]);
    }
}

void CFAutoEncoderComputationST::UpdateNetwork() {
    UpdateNetworkInputToHidden();
    UpdateNetworkHiddenToOutput();
}

void CFAutoEncoderComputationST::UpdateNetworkInputToHidden() {
    const auto& uv = mData->GetUserVector(mCurrentSampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    double learningRate = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
    for (int i = 0; i < mAutoEncoder->mNumHiddenUnit; ++i) {
        for (int jj = 0; jj < size; ++jj) {
            int itemId = data[jj].ItemId();
            int rating = static_cast<int>(data[jj].Rating());
            int j = itemId*5+rating;
            double weightGrad1 = mDelta1[i];
            mAutoEncoder->mWeight1[i][j] -= learningRate*(weightGrad1 + lambda*mAutoEncoder->mWeight1[i][j]);
        }
        double biasGradient1 = mDelta1[i];
        mAutoEncoder->mBias1[i] -= learningRate * biasGradient1;
    }
}

void CFAutoEncoderComputationST::UpdateNetworkHiddenToOutput() {
    const auto& uv = mData->GetUserVector(mCurrentSampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    double learningRate = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
    for (int ii = 0; ii < size; ++ii) {
        int itemId = data[ii].ItemId();
        for (int i = itemId*5; i < itemId*5+5; ++i) {
            for (int j = 0; j < mAutoEncoder->mNumHiddenUnit; ++j) {
                double weightGrad2 = mDelta2[i] * mActivation1[j];
                mAutoEncoder->mWeight2[i][j] -= learningRate*(weightGrad2 + lambda*mAutoEncoder->mWeight2[i][j]);
            }
            double biasGrad2 = mDelta2[i];
            mAutoEncoder->mBias2[i] -= learningRate * biasGrad2;
        }
    }
}

void CFAutoEncoderComputationST::ComputeTotalLoss() {
    mDataLoss = 0.0;
    for (int i = 0; i < mData->NumUser(); ++i) {
        mDataLoss += mSampleLosses[i];
    }
    mDataLoss /= 2.0 * mData->NumUser();
    mRegLoss = NormFSqr(mAutoEncoder->mWeight1) + NormFSqr(mAutoEncoder->mWeight2);
    mRegLoss *= 0.5 * mTrainOption->Lambda();
    mTotalLoss = mDataLoss + mRegLoss;
}

//============================================================

void CFAutoEncoderComputationMT::Train(CFAutoEncoder *autoEncoder, const CFAE::TrainOption *trainOption,
        const RatingMatrixAsUsers<> *data) {
    mAutoEncoder = autoEncoder;
    mTrainOption = trainOption;
    mData = data;
    if (mTrainOption->IsRandomInit()) {
        RandomInit();
    }
//    InitResource();
//    mThreadId = 0;
//    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mTrainOption->NumThread(), 1);
//    mScheduler->Start();
//    mScheduler->WaitFinish();
//    delete mScheduler;
}
/*
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
                Log::I("ml", "[MT]iter %d, dataLoss=%lf, regLoss=%lf, "
                       "totalLoss=%lf, time=%.2lfs", task.iter, mDataLoss, mRegLoss, mTotalLoss,
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
*/
/*

void CFAutoEncoder::AllocateThreadMemory() {
    LOG(INFO) << "allocate all memory needed by training thread.";
    mWeight1MT.clear();    mBias1MT.clear();
    mWeight2MT.clear();    mBias2MT.clear();
    mAct1MT.clear();    mAct2MT.clear();
    mAct3MT.clear();    mTargetMT.clear();
    mDelta1MT.clear();    mDelta2MT.clear();
    mWeight1GradTh.clear();    mBias1GradTh.clear();
    mWeight2GradTh.clear();    mBias2GradTh.clear();
    mWeight1UpdateTh.clear();    mBias1UpdateTh.clear();
    mWeight2UpdateTh.clear();    mBias2UpdateTh.clear();
    for (int tid = 0; tid < mNumThread; ++tid) {
        double **weight1, *bias1, **weight2, *bias2;
        AllocateMat(&weight1, mHiddenUnit, mInputUnit);
        AllocateVec(&bias1, mHiddenUnit);
        AllocateMat(&weight2, mOutputUnit, mHiddenUnit);
        AllocateVec(&bias2, mOutputUnit);
        mWeight1MT.push_back(weight1);
        mBias1MT.push_back(bias1);
        mWeight2MT.push_back(weight2);
        mBias2MT.push_back(bias2);
        double *act1, *act2, *act3, *target;
        AllocateVec(&act1, mInputUnit);
        AllocateVec(&act2, mHiddenUnit);
        AllocateVec(&act3, mOutputUnit);
        AllocateVec(&target, mOutputUnit);
        mAct1MT.push_back(act1);
        mAct2MT.push_back(act2);
        mAct3MT.push_back(act3);
        mTargetMT.push_back(target);
        double *delta1, *delta2;
        AllocateVec(&delta1, mHiddenUnit);
        AllocateVec(&delta2, mOutputUnit);
        mDelta1MT.push_back(delta1);
        mDelta2MT.push_back(delta2);
        double **weight1Grad, *bias1Grad, **weight2Grad, *bias2Grad;
        AllocateMat(&weight1Grad, mHiddenUnit, mInputUnit);
        AllocateVec(&bias1Grad, mHiddenUnit);
        AllocateMat(&weight2Grad, mOutputUnit, mHiddenUnit);
        AllocateVec(&bias2Grad, mOutputUnit);
        mWeight1GradTh.push_back(weight1Grad);
        mBias1GradTh.push_back(bias1Grad);
        mWeight2GradTh.push_back(weight2Grad);
        mBias2GradTh.push_back(bias2Grad);
        double **weight1Update, *bias1Update, **weight2Update, *bias2Update;
        AllocateMat(&weight1Update, mHiddenUnit, mInputUnit);
        AllocateVec(&bias1Update, mHiddenUnit);
        AllocateMat(&weight2Update, mOutputUnit, mHiddenUnit);
        AllocateVec(&bias2Update, mOutputUnit);
        mWeight1UpdateTh.push_back(weight1Update);
        mBias1UpdateTh.push_back(bias1Update);
        mWeight2UpdateTh.push_back(weight2Update);
        mBias2UpdateTh.push_back(bias2Update);
    }
    LOG(INFO) << "allocate memory done.";
}

void CFAutoEncoder::FreeThreadMemory() {
    LOG(INFO) << "free all memory used by training thread.";
    for (int tid = 0; tid < mNumThread; ++tid) {
        FreeMat(&mWeight1MT[tid], mHiddenUnit, mInputUnit);
        FreeVec(&mBias1MT[tid], mHiddenUnit);
        FreeMat(&mWeight2MT[tid], mOutputUnit, mHiddenUnit);
        FreeVec(&mBias2MT[tid], mOutputUnit);
        FreeVec(&mAct1MT[tid], mInputUnit);
        FreeVec(&mAct2MT[tid], mHiddenUnit);
        FreeVec(&mAct3MT[tid], mOutputUnit);
        FreeVec(&mTargetMT[tid], mOutputUnit);
        FreeVec(&mDelta1MT[tid], mHiddenUnit);
        FreeVec(&mDelta2MT[tid], mOutputUnit);
        FreeMat(&mWeight1GradTh[tid], mHiddenUnit, mInputUnit);
        FreeVec(&mBias1GradTh[tid], mHiddenUnit);
        FreeMat(&mWeight2GradTh[tid], mOutputUnit, mHiddenUnit);
        FreeVec(&mBias2GradTh[tid], mOutputUnit);
        FreeMat(&mWeight1UpdateTh[tid], mHiddenUnit, mInputUnit);
        FreeVec(&mBias1UpdateTh[tid], mHiddenUnit);
        FreeMat(&mWeight2UpdateTh[tid], mOutputUnit, mHiddenUnit);
        FreeVec(&mBias2UpdateTh[tid], mOutputUnit);
    }
    LOG(INFO) << "free memory done.";
}

void CFAutoEncoder::RandomInit() {
    double mean = 0.0, std = 0.1;
    LOG(INFO) << "random init weight and bias by gauss(" << mean << "," << std << ")";
    Random& rnd = Random::Instance();
    for (int i = 0; i < mHiddenUnit; ++i) {
        for (int j = 0; j < mInputUnit; ++j) {
            mWeight1[i][j] = rnd.Gauss(mean, std);
        }
    }
    for (int i = 0; i < mHiddenUnit; ++i) {
        mBias1[i] = rnd.Gauss(mean, std);
    }
    for (int i = 0; i < mOutputUnit; ++i) {
        for (int j = 0; j < mHiddenUnit; ++j) {
            mWeight2[i][j] = rnd.Gauss(mean, std);
        }
    }
    for (int i = 0; i < mOutputUnit; ++i) {
        mBias2[i] = rnd.Gauss(mean, std);
    }
    LOG(INFO) << "init weight and bias done.";
}

void CFAutoEncoder::TrainSGDThreadWork(int taskId) {
    int batchSize = mpDatamodel->NumSamples() / mNumThread;
    int begin = taskId * batchSize;
    int end = (taskId + 1) * batchSize;
    double **weight1 = mWeight1MT[taskId];
    double *bias1 = mBias1MT[taskId];
    double **weight2 = mWeight2MT[taskId];
    double *bias2 = mBias2MT[taskId];
    CopyMat(mWeight1, weight1, mHiddenUnit, mInputUnit);
    CopyVec(mBias1, bias1, mHiddenUnit);
    CopyMat(mWeight2, weight2, mOutputUnit, mHiddenUnit);
    CopyVec(mBias2, bias2, mOutputUnit);
    double *act1 = mAct1MT[taskId];
    double *act2 = mAct2MT[taskId];
    double *act3 = mAct3MT[taskId];
    double *target = mTargetMT[taskId];
    double *delta1 = mDelta1MT[taskId];
    double *delta2 = mDelta2MT[taskId];
    double **weight1Grad = mWeight1GradTh[taskId];
    double *bias1Grad = mBias1GradTh[taskId];
    double **weight2Grad = mWeight2GradTh[taskId];
    double *bias2Grad = mBias2GradTh[taskId];
    double **weight1Update = mWeight1UpdateTh[taskId];
    double *bias1Update = mBias1UpdateTh[taskId];
    double **weight2Update = mWeight2UpdateTh[taskId];
    double *bias2Update = mBias2UpdateTh[taskId];
    for (int i = begin; i < end; ++i) {
        int idx = mShuffledSampleIdx[i];
        CopyVec(mpDatamodel->Feature(idx).Data(), act1, mInputUnit);
        CopyVec(mpDatamodel->Feature(idx).Data(), target, mOutputUnit);
        Forward(act1, weight1, bias1, weight2, bias2, act2, act3);
        Backward(act2, act3, target, weight2, delta1, delta2);
        ComputeStochasticGradient(delta1, delta2, act1, act2,
                weight1Grad, bias1Grad, weight2Grad, bias2Grad);
        UpdateNetwork(weight1Grad, bias1Grad, weight1Update, bias1Update, weight1, bias1,
                weight2Grad, bias2Grad, weight2Update, bias2Update, weight2, bias2);
    }
}

void CFAutoEncoder::ComputeStochasticGradient(double *delta1, double *delta2, double *act1, double *act2,
        double **weight1Grad, double *bias1Grad, double **weight2Grad, double *bias2Grad) {
    for (int i = 0; i < mHiddenUnit; ++i) {
        for (int j = 0; j < mInputUnit; ++j) {
            weight1Grad[i][j] = delta1[i] * act1[j];
        }
        bias1Grad[i] = delta1[i];
    }
}

void CFAutoEncoder::UpdateNetwork(double **weight1Grad, double *bias1Grad, double **weight1Update,
        double *bias1Update, double **weight1, double *bias1, double **weight2Grad, double *bias2Grad,
        double **weight2Update, double *bias2Update, double **weight2, double *bias2) {
    for (int i = 0; i < mHiddenUnit; ++i) {
        for (int j = 0; j < mInputUnit; ++j) {
            weight1Update[i][j] = mLearningRate * (weight1Grad[i][j] + mLambda * weight1[i][j])
                                + mMomentum * weight1Update[i][j];
            weight1[i][j] -= weight1Update[i][j];
        }
        bias1Update[i] = mLearningRate * bias1Grad[i] + mMomentum * bias1Update[i];
        bias1[i] -= bias1Update[i];
    }
    for (int i = 0; i < mOutputUnit; ++i) {
        for (int j = 0; j < mHiddenUnit; ++j) {
            weight2Update[i][j] = mLearningRate * (weight2Grad[i][j] + mLambda * weight2[i][j])
                                + mMomentum * weight2Update[i][j];
            weight2[i][j] -= weight2Update[i][j];
        }
        bias2Update[i] = mLearningRate * bias2Grad[i] + mMomentum * bias2Update[i];
        bias2[i] -= bias2Update[i];
    }
}

void CFAutoEncoder::CombineSGDThreadWorkerResult() {
    for (int i = 0; i < mHiddenUnit; ++i) {
        for (int j = 0; j < mInputUnit; ++j) {
            double sum = 0.0;
            for (int tid = 0; tid < mNumThread; ++tid) {
                sum += mWeight1MT[tid][i][j];
            }
            mWeight1[i][j] = sum / mNumThread;
        }
    }
    for (int i = 0; i < mHiddenUnit; ++i) {
        double sum = 0;
        for (int tid = 0; tid < mNumThread; ++tid) {
            sum += mBias1MT[tid][i];
        }
        mBias1[i] = sum / mNumThread;
    }
    for (int i = 0; i < mOutputUnit; ++i) {
        for (int j = 0; j < mHiddenUnit; ++j) {
            double sum = 0.0;
            for (int tid = 0; tid < mNumThread; ++tid) {
                sum += mWeight2MT[tid][i][j];
            }
            mWeight2[i][j] = sum / mNumThread;
        }
    }
    for (int i = 0; i < mOutputUnit; ++i) {
        double sum = 0;
        for (int tid = 0; tid < mNumThread; ++tid) {
            sum += mBias2MT[tid][i];
        }
        mBias2[i] = sum / mNumThread;
    }
}

bool CFAutoEncoder::IterationCheckStop() {
    double costFun = ComputeTotalCostFun();
    LOG(INFO) << "total cost fun = " << costFun;
    if (mCurrentCostFun > costFun) {
        mCurrentCostFun = costFun;
        return false;
    } else {
        return true;
    }
}

double CFAutoEncoder::ComputeTotalCostFun() {
    std::vector<double> dataCosts(mNumThread);
    std::vector<std::thread*> threads;
    for (int tid = 0; tid < mNumThread; ++tid) {
        std::thread *worker = new std::thread(&AutoEncoder::ComputeTotalCostFunThreadWork,
                this, tid, &dataCosts[tid]);
        threads.push_back(worker);
    }
    for (int tid = 0; tid < mNumThread; ++tid) {
        std::thread *worker = threads[tid];
        worker->join();
        delete threads[tid];
    }
    double dataCost = 0.0;
    for (int tid = 0; tid < mNumThread; ++tid) {
        dataCost += dataCosts[tid];
    }
    dataCost /= 2.0 * mpDatamodel->NumSamples();

    double regCost = 0.0;
    for (int i = 0; i < mHiddenUnit; ++i) {
        for (int j = 0; j < mInputUnit; ++j) {
            regCost += Math::Sqr(mWeight1[i][j]);
        }
    }
    for (int i = 0; i < mOutputUnit; ++i) {
        for (int j = 0; j < mHiddenUnit; ++j) {
            regCost += Math::Sqr(mWeight2[i][j]);
        }
    }
    regCost *= 0.5 * mLambda;
    return dataCost + regCost;
}

void CFAutoEncoder::ComputeTotalCostFunThreadWork(int taskId, double* error) {
    int batchSize = mpDatamodel->NumSamples() / mNumThread;
    int begin = taskId * batchSize;
    int end = (taskId != mNumThread - 1)?(taskId + 1) * batchSize : mpDatamodel->NumSamples();
    double *act1 = mAct1MT[taskId];
    double *act2 = mAct2MT[taskId];
    double *act3 = mAct3MT[taskId];
    double *target = mTargetMT[taskId];
    *error = 0.0;
    for (int idx = begin; idx < end; ++idx) {
        CopyVec(mpDatamodel->Feature(idx).Data(), act1, mInputUnit);
        CopyVec(mpDatamodel->Feature(idx).Data(), target, mOutputUnit);
        Forward(act1, mWeight1, mBias1, mWeight2, mBias2, act2, act3);
        double sum = 0.0;
        for (int i = 0; i < mOutputUnit; ++i) {
            sum += Math::Sqr(act3[i]-target[i]);
        }
        *error += sum;
    }
}

*/
} //~ namespace longan
