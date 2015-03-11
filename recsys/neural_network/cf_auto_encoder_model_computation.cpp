/*
 * cf_auto_encoder_computation.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_model_computation.h"
#include "ml/base/math.h"
#include "common/common.h"

namespace longan {

namespace CFAE {

void ModelComputation::ComputeModelUser(const TrainOption* option, const RatingMatUsers* trainData,
        const RatingMatUsers* validateData, Model* model) {
    mTrainOption = option;
    mTrainDataUsers = trainData;
    mValidateDataUsers = validateData;
    mModel = model;
    TrainSparseLayerUser();
    EncodeSamplesUser();
}

void ModelComputation::ComputeModelItem(const TrainOption* option, const RatingMatItems* trainData,
        const RatingMatItems* validateData, Model* model) {
    mTrainOption = option;
    mTrainDataItems = trainData;
    mValidateDataItems = validateData;
    mModel = model;
    TrainSparseLayerItem();
    EncodeSamplesItem();
}

void ModelComputation::TrainSparseLayerUser() {
    TrainSparseLayerDelegate *delegate = nullptr;
    if (mTrainOption->Accelerate()) {
        delegate = new TrainSparseLayerDelegateMT();
    } else {
        delegate = new TrainSparseLayerDelegateST();
    }
    delegate->TrainSparseLayerUser(mTrainOption, mTrainDataUsers, mValidateDataUsers, mModel);
    delete delegate;
}

void ModelComputation::TrainSparseLayerItem() {
    TrainSparseLayerDelegate *delegate = nullptr;
    if (mTrainOption->Accelerate()) {
        delegate = new TrainSparseLayerDelegateMT();
    } else {
        delegate = new TrainSparseLayerDelegateST();
    }
    delegate->TrainSparseLayerItem(mTrainOption, mTrainDataItems, mValidateDataItems, mModel);
    delete delegate;
}

void ModelComputation::EncodeSamplesUser() {
    EncodeSamplesDelegateST *delegate = new EncodeSamplesDelegateST();
    delegate->EncodeUser(mTrainDataUsers, mModel);
    delete delegate;
}

void ModelComputation::EncodeSamplesItem() {
    EncodeSamplesDelegateST *delegate = new EncodeSamplesDelegateST();
    delegate->EncodeItem(mTrainDataItems, mModel);
    delete delegate;
}

void TrainSparseLayerDelegate::ForwardUser(int sampleId, Vector64F& activation1, Vector64F& activation2) {
    const UserVec& uv = mTrainDataUsers->GetUserVector(sampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    const Matrix64F& encodeWeight = mModel->mEncodeWeights[0];
    const Vector64F& encodeBias = mModel->mEncodeBiases[0];
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        double sum = encodeBias[i];
        for (int j = 0; j < size; ++j) {
            int itemId = data[j].ItemId();
            float act0 = data[j].Rating();
            sum += encodeWeight[i][itemId] * act0;
        }
        activation1[i] = Math::Sigmoid(sum);
    }
    const Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    const Vector64F& decodeBias = mModel->mDecodeBiases[0];
    for (int i = 0; i < size; ++i) {
        int itemId = data[i].ItemId();
        double sum = decodeBias[itemId];
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            sum += decodeWeight[itemId][j] * activation1[j];
        }
        activation2[itemId] = Math::Sigmoid(sum);
    }
}

void TrainSparseLayerDelegate::ForwardItem(int sampleId, Vector64F& activation1, Vector64F& activation2) {
    const ItemVec& iv = mTrainDataItems->GetItemVector(sampleId);
    const UserRating *data = iv.Data();
    int size = iv.Size();
    const Matrix64F& encodeWeight = mModel->mEncodeWeights[0];
    const Vector64F& encodeBias = mModel->mEncodeBiases[0];
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        double sum = encodeBias[i];
        for (int j = 0; j < size; ++j) {
            int userId = data[j].UserId();
            float act0 = data[j].Rating();
            sum += encodeWeight[i][userId] * act0;
        }
        activation1[i] = Math::Sigmoid(sum);
    }
    const Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    const Vector64F& decodeBias = mModel->mDecodeBiases[0];
    for (int i = 0; i < size; ++i) {
        int userId = data[i].UserId();
        double sum = decodeBias[userId];
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            sum += decodeWeight[userId][j] * activation1[j];
        }
        activation2[userId] = Math::Sigmoid(sum);
    }
}

void TrainSparseLayerDelegate::BackwardUser(int sampleId, const Vector64F& activation1,
        const Vector64F& activation2, Vector64F& delta1, Vector64F& delta2) {
    const UserVec& uv = mTrainDataUsers->GetUserVector(sampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    mSampleLoss[sampleId] = 0.0;
    for (int i = 0; i < size; ++i) {
        int itemId = data[i].ItemId();
        float target = data[i].Rating();
        double error = activation2[itemId] - target;
        delta2[itemId] = error * activation2[itemId] * (1.0 - activation2[itemId]);
        mSampleLoss[sampleId] += Math::Sqr(error);
    }
    const Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        double sum = 0.0;
        for (int j = 0; j < size; ++j) {
            int itemId = data[j].ItemId();
            sum += decodeWeight[itemId][i] * delta2[itemId];
        }
        delta1[i] = sum * activation1[i] * (1.0 - activation1[i]);
    }
}

void TrainSparseLayerDelegate::BackwardItem(int sampleId, const Vector64F& activation1,
        const Vector64F& activation2, Vector64F& delta1, Vector64F& delta2) {
    const ItemVec& iv = mTrainDataItems->GetItemVector(sampleId);
    const UserRating *data = iv.Data();
    int size = iv.Size();
    mSampleLoss[sampleId] = 0.0;
    for (int i = 0; i < size; ++i) {
        int userId = data[i].UserId();
        float target = data[i].Rating();
        double error = activation2[userId] - target;
        delta2[userId] = error * activation2[userId] * (1.0 - activation2[userId]);
        mSampleLoss[sampleId] += Math::Sqr(error);
    }
    const Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        double sum = 0.0;
        for (int j = 0; j < size; ++j) {
            int userId = data[j].UserId();
            sum += decodeWeight[userId][i] * delta2[userId];
        }
        delta1[i] = sum * activation1[i] * (1.0 - activation1[i]);
    }
}

void TrainSparseLayerDelegate::UpdateUser(int sampleId, const Vector64F& activation1,
        const Vector64F& delta1, const Vector64F& delta2) {
    const UserVec& uv = mTrainDataUsers->GetUserVector(sampleId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    double lr = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
    Matrix64F& encodeWeight = mModel->mEncodeWeights[0];
    Vector64F& encodeBias = mModel->mEncodeBiases[0];
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        for (int j = 0; j < size; ++j) {
            int itemId = data[j].ItemId();
            float act0 = data[j].Rating();
            double weightGrad = delta1[i] * act0;
            encodeWeight[i][itemId] -=
                lr * (weightGrad + lambda * encodeWeight[i][itemId]);
        }
        double biasGrad = delta1[i];
        encodeBias[i] -= lr * biasGrad;
    }
    Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    Vector64F& decodeBias = mModel->mDecodeBiases[0];
    for (int i = 0; i < size; ++i) {
        int itemId = data[i].ItemId();
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            double weightGrad = delta2[itemId] * activation1[j];
            decodeWeight[itemId][j] -=
                    lr * (weightGrad + lambda * decodeWeight[itemId][j]);
        }
        double biasGrad = delta2[itemId];
        decodeBias[itemId] -= lr * biasGrad;
    }
}

void TrainSparseLayerDelegate::UpdateItem(int sampleId, const Vector64F& activation1,
        const Vector64F& delta1, const Vector64F& delta2) {
    const ItemVec& iv = mTrainDataItems->GetItemVector(sampleId);
    const UserRating *data = iv.Data();
    int size = iv.Size();
    double lr = mTrainOption->LearningRate();
    double lambda = mTrainOption->Lambda();
    Matrix64F& encodeWeight = mModel->mEncodeWeights[0];
    Vector64F& encodeBias = mModel->mEncodeBiases[0];
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        for (int j = 0; j < size; ++j) {
            int userId = data[j].UserId();
            float act0 = data[j].Rating();
            double weightGrad = delta1[i] * act0;
            encodeWeight[i][userId] -=
                lr * (weightGrad + lambda * encodeWeight[i][userId]);
        }
        double biasGrad = delta1[i];
        encodeBias[i] -= lr * biasGrad;
    }
    Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    Vector64F& decodeBias = mModel->mDecodeBiases[0];
    for (int i = 0; i < size; ++i) {
        int userId = data[i].UserId();
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            double weightGrad = delta2[userId] * activation1[j];
            decodeWeight[userId][j] -=
                    lr * (weightGrad + lambda * decodeWeight[userId][j]);
        }
        double biasGrad = delta2[userId];
        decodeBias[userId] -= lr * biasGrad;
    }
}

double TrainSparseLayerDelegate::ComputeDataLoss() {
    double loss = 0.0;
    for (int sampleId = 0; sampleId < mNumSample; ++sampleId) {
        loss += mSampleLoss[sampleId];
    }
    return loss;
}

double TrainSparseLayerDelegate::ComputeRegLoss() {
    double loss = mTrainOption->Lambda() *
            (NormFSqr(mModel->mEncodeWeights[0]) + NormFSqr(mModel->mDecodeWeights[0]));
    return loss;
}

double TrainSparseLayerDelegate::ComputeValidateLoss() {
    if (mModel->mParameter->CodeType() == Parameter::CodeType_User) {
        return ComputeValidateLossUser();
    } else if (mModel->mParameter->CodeType() == Parameter::CodeType_Item) {
        return ComputeValidateLossItem();
    } else {
        return 0.0;
    }
}

double TrainSparseLayerDelegate::ComputeValidateLossUser() {
    double loss = 0.0;
    const Matrix64F& encodeWeight = mModel->mEncodeWeights[0];
    const Vector64F& encodeBias = mModel->mEncodeBiases[0];
    const Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    const Vector64F& decodeBias = mModel->mDecodeBiases[0];
    Vector64F activation1(mNumHiddenUnit);
    for (int sampleId = 0; sampleId < mNumSample; ++sampleId) {
        const UserVec& uv = mTrainDataUsers->GetUserVector(sampleId);
        const ItemRating *data = uv.Data();
        int size = uv.Size();
        const UserVec& uv2 = mValidateDataUsers->GetUserVector(sampleId);
        const ItemRating *data2 = uv2.Data();
        int size2 = uv2.Size();
        for (int i = 0; i < mNumHiddenUnit; ++i) {
            double sum = encodeBias[i];
            for (int j = 0; j < size; ++j) {
                int itemId = data[j].ItemId();
                float act0 = data[j].Rating();
                sum += encodeWeight[i][itemId] * act0;
            }
            for (int j = 0; j < size2; ++j) {
                int itemId = data2[j].ItemId();
                float act0 = data2[j].Rating();
                sum += encodeWeight[i][itemId] * act0;
            }
            activation1[i] = Math::Sigmoid(sum);
        }
        for (int i = 0; i < size2; ++i) {
            int itemId = data2[i].ItemId();
            double sum = decodeBias[itemId];
            for (int j = 0; j < mNumHiddenUnit; ++j) {
                sum += decodeWeight[itemId][j] * activation1[j];
            }
            double error = Math::Sigmoid(sum) - data2[i].Rating();
            loss += Math::Sqr(error);
        }
    }
    return loss;
}

double TrainSparseLayerDelegate::ComputeValidateLossItem() {
    double loss = 0.0;
    const Matrix64F& encodeWeight = mModel->mEncodeWeights[0];
    const Vector64F& encodeBias = mModel->mEncodeBiases[0];
    const Matrix64F& decodeWeight = mModel->mDecodeWeights[0];
    const Vector64F& decodeBias = mModel->mDecodeBiases[0];
    Vector64F activation1(mNumHiddenUnit);
    for (int sampleId = 0; sampleId < mNumSample; ++sampleId) {
        const ItemVec& iv = mTrainDataItems->GetItemVector(sampleId);
        const UserRating *data = iv.Data();
        int size = iv.Size();
        const ItemVec& iv2 = mValidateDataItems->GetItemVector(sampleId);
        const UserRating *data2 = iv2.Data();
        int size2 = iv2.Size();
        for (int i = 0; i < mNumHiddenUnit; ++i) {
            double sum = encodeBias[i];
            for (int j = 0; j < size; ++j) {
                int userId = data[j].UserId();
                float act0 = data[j].Rating();
                sum += encodeWeight[i][userId] * act0;
            }
            for (int j = 0; j < size2; ++j) {
                int userId = data2[j].UserId();
                float act0 = data2[j].Rating();
                sum += encodeWeight[i][userId] * act0;
            }
            activation1[i] = Math::Sigmoid(sum);
        }
        for (int i = 0; i < size2; ++i) {
            int userId = data2[i].UserId();
            double sum = decodeBias[userId];
            for (int j = 0; j < mNumHiddenUnit; ++j) {
                sum += decodeWeight[userId][j] * activation1[j];
            }
            double error = Math::Sigmoid(sum) - data2[i].Rating();
            loss += Math::Sqr(error);
        }
    }
    return loss;
}

void TrainSparseLayerDelegateST::TrainSparseLayerUser(const TrainOption *option,
        const RatingMatUsers *trainData, const RatingMatUsers *validateData, Model *model) {
    mTrainOption = option;
    mTrainDataUsers = trainData;
    mValidateDataUsers = validateData;
    mModel = model;
    mNumInputUnit = model->mNumInputUnit;
    mNumHiddenUnit = model->mParameter->NumLayerUnit(0);
    mNumSample = model->mNumSample;
    InitResource();
    std::vector<int> order(mNumSample);
    ArrayHelper::FillRange(order.data(), order.size());
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(order.data(), order.size());
        for (int i = 0; i < mNumSample; ++i) {
            if (i % 1000 == 0) Log::Console("recsys", "SGD on sample %d/%d", i, mNumSample);
            int sampleId = order[i];
            ForwardUser(sampleId, mActivation1, mActivation2);
            BackwardUser(sampleId, mActivation1, mActivation2, mDelta1, mDelta2);
            UpdateUser(sampleId, mActivation1, mDelta1, mDelta2);
        }
        if (mTrainOption->MonitorIteration() && iter % mTrainOption->MonitorIterationStep() == 0) {
            double dataLoss = ComputeDataLoss();
            double regLoss = ComputeRegLoss();
            double trainLoss = dataLoss + regLoss;
            double validateLoss = ComputeValidateLossUser();
            Log::I("recsys", "iteration %d/%d done. \n"
                   "data loss = %lf\t reg loss = %lf\t train loss = %lf\t validate loss = %lf",
                   iter, mTrainOption->Iterations(),
                   dataLoss, regLoss, trainLoss, validateLoss);
        }
    }
}

void TrainSparseLayerDelegateST::TrainSparseLayerItem(const TrainOption *option,
        const RatingMatItems *trainData, const RatingMatItems *validateData, Model *model) {
    mTrainOption = option;
    mTrainDataItems = trainData;
    mValidateDataItems = validateData;
    mModel = model;
    mNumInputUnit = model->mNumInputUnit;
    mNumHiddenUnit = model->mParameter->NumLayerUnit(0);
    mNumSample = model->mNumSample;
    InitResource();
    std::vector<int> order(mNumSample);
    ArrayHelper::FillRange(order.data(), order.size());
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(order.data(), order.size());
        for (int i = 0; i < mNumSample; ++i) {
            if (i % 1000 == 0) Log::Console("recsys", "SGD on sample %d/%d", i, mNumSample);
            int sampleId = order[i];
            ForwardItem(sampleId, mActivation1, mActivation2);
            BackwardItem(sampleId, mActivation1, mActivation2, mDelta1, mDelta2);
            UpdateItem(sampleId, mActivation1, mDelta1, mDelta2);
        }
        if (mTrainOption->MonitorIteration() && iter % mTrainOption->MonitorIterationStep() == 0) {
            double dataLoss = ComputeDataLoss();
            double regLoss = ComputeRegLoss();
            double trainLoss = dataLoss + regLoss;
            double validateLoss = ComputeValidateLossItem();
            Log::I("recsys", "iteration %d/%d done. \n"
                    "data loss = %lf\t reg loss = %lf\t train loss = %lf\t validate loss = %lf",
                    iter, mTrainOption->Iterations(),
                    dataLoss, regLoss, trainLoss, validateLoss);
        }
    }
}

void TrainSparseLayerDelegateST::InitResource() {
    mActivation1 = std::move(Vector64F(mNumHiddenUnit));
    mActivation2 = std::move(Vector64F(mNumInputUnit));
    mDelta1 = std::move(Vector64F(mNumHiddenUnit));
    mDelta2 = std::move(Vector64F(mNumInputUnit));
    mSampleLoss.resize(mNumSample);
}


void TrainSparseLayerDelegateMT::TrainSparseLayerUser(const TrainOption* option,
        const RatingMatUsers* trainData, const RatingMatUsers* validateData, Model* model) {
    mTrainOption = option;
    mTrainDataUsers = trainData;
    mValidateDataUsers = validateData;
    mModel = model;
    mNumInputUnit = model->mNumInputUnit;
    mNumHiddenUnit = model->mParameter->NumLayerUnit(0);
    mNumSample = model->mNumSample;
    InitResource();
    mThreadId = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mTrainOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void TrainSparseLayerDelegateMT::TrainSparseLayerItem(const TrainOption* option,
        const RatingMatItems* trainData, const RatingMatItems* validateData, Model* model) {
    mTrainOption = option;
    mTrainDataItems = trainData;
    mValidateDataItems = validateData;
    mModel = model;
    mNumInputUnit = model->mNumInputUnit;
    mNumHiddenUnit = model->mParameter->NumLayerUnit(0);
    mNumSample = model->mNumSample;
    InitResource();
    mThreadId = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mTrainOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void TrainSparseLayerDelegateMT::InitResource() {
    int numThread = mTrainOption->NumThread();
    mActivation1.resize(numThread);
    mActivation2.resize(numThread);
    mDelta1.resize(numThread);
    mDelta2.resize(numThread);
    for (int tid = 0; tid < numThread; ++tid) {
        mActivation1[tid] = std::move(Vector64F(mNumHiddenUnit));
        mActivation2[tid] = std::move(Vector64F(mNumInputUnit));
        mDelta1[tid] = std::move(Vector64F(mNumHiddenUnit));
        mDelta2[tid] = std::move(Vector64F(mNumInputUnit));
    }
    mSampleLoss.resize(mNumSample);
}

void TrainSparseLayerDelegateMT::ProducerRun() {
    std::vector<int> order(mNumSample);
    ArrayHelper::FillRange(order.data(), order.size());
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(order.data(), order.size());
        for (int i = 0; i < order.size(); ++i) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->ProducerPutTask(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            Task task;
            task.type = TaskTypeCompute;
            task.sampleId = order[i];
            task.iter = iter;
            currentBundle->push_back(task);
        }
        if (mTrainOption->MonitorIteration() && iter % mTrainOption->MonitorIterationStep() == 0) {
           Task task;
           task.type = TaskTypeMonitor;
           task.iter = iter;
           currentBundle->push_back(task);
        }
    }
    mScheduler->ProducerPutTask(currentBundle);
    mScheduler->ProducerDone();
}

void TrainSparseLayerDelegateMT::WorkerRun(int threadId) {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            if (task.type == TaskTypeCompute) {
                SGDOnSample(threadId, task.sampleId);
            } else if (task.type == TaskTypeMonitor) {
                std::unique_lock<std::mutex> lock(mMonitorIterationMutex);
                double dataLoss = ComputeDataLoss();
                double regLoss = ComputeRegLoss();
                double trainLoss = dataLoss + regLoss;
                double validateLoss = ComputeValidateLoss();
                Log::I("recsys", "iteration %d/%d done. \n"
                       "data loss = %lf\t reg loss = %lf\t train loss = %lf\t validate loss = %lf",
                        task.iter, mTrainOption->Iterations(),
                        dataLoss, regLoss, trainLoss, validateLoss);
            }
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void TrainSparseLayerDelegateMT::SGDOnSample(int threadId, int sampleId) {
    if (mModel->mParameter->CodeType() == Parameter::CodeType_User) {
        ForwardUser(sampleId, mActivation1[threadId], mActivation2[threadId]);
        BackwardUser(sampleId, mActivation1[threadId], mActivation2[threadId],
                mDelta1[threadId], mDelta2[threadId]);
        UpdateUser(sampleId, mActivation1[threadId], mDelta1[threadId], mDelta2[threadId]);
    } else if (mModel->mParameter->CodeType() == Parameter::CodeType_Item) {
        ForwardItem(sampleId, mActivation1[threadId], mActivation2[threadId]);
        BackwardItem(sampleId, mActivation1[threadId], mActivation2[threadId],
                mDelta1[threadId], mDelta2[threadId]);
        UpdateItem(sampleId, mActivation1[threadId], mDelta1[threadId], mDelta2[threadId]);
    }
}

void TrainSparseLayerDelegateMT::ConsumerRun() {
    int numProcessedSample = 0;
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        if (mTrainOption->MonitorProgress()) {
            for (int i = 0; i < currentBundle->size(); ++i) {
                Task& task = currentBundle->at(i);
                ++numProcessedSample;
                if (numProcessedSample % 1000 == 0) {
                    Log::Console("recsys", "at iter %d/%d, SGD on sample %d/%d",
                            task.iter, mTrainOption->Iterations(),
                            numProcessedSample % mNumSample, mNumSample);
                }
            }
        }
        delete currentBundle;
    }
    mScheduler->ConsumerDone();
}

void EncodeSamplesDelegateST::EncodeUser(const RatingMatUsers* trainData, Model* model) {
    int numHiddenUnit = model->mParameter->NumLayerUnit(0);
    const Matrix64F& encodeWeight = model->mEncodeWeights[0];
    const Vector64F& encodeBias = model->mEncodeBiases[0];
    for (int uid = 0; uid < trainData->NumUser(); ++uid) {
        Vector64F& code = model->mCodes[uid];
        assert(code.Size() == numHiddenUnit);
        const UserVec& uv = trainData->GetUserVector(uid);
        const ItemRating *data = uv.Data();
        int size = uv.Size();
        for (int i = 0; i < numHiddenUnit; ++i) {
            double sum = encodeBias[i];
            for (int j = 0; j < size; ++j) {
                int itemId = data[j].ItemId();
                float act0 = data[j].Rating();
                sum += encodeWeight[i][itemId] * act0;
            }
            code[i] = Math::Sigmoid(sum);
        }
    }
}

void EncodeSamplesDelegateST::EncodeItem(const RatingMatItems *trainData, Model *model) {
    int numHiddenUnit = model->mParameter->NumLayerUnit(0);
    const Matrix64F& encodeWeight = model->mEncodeWeights[0];
    const Vector64F& encodeBias = model->mEncodeBiases[0];
    for (int iid = 0; iid < trainData->NumItem(); ++iid) {
        Vector64F& code = model->mCodes[iid];
        assert(code.Size() == numHiddenUnit);
        const ItemVec& iv = trainData->GetItemVector(iid);
        const UserRating *data = iv.Data();
        int size = iv.Size();
        for (int i = 0; i < numHiddenUnit; ++i) {
            double sum = encodeBias[i];
            for (int j = 0; j < size; ++j) {
                int userId = data[j].UserId();
                float act0 = data[j].Rating();
                sum += encodeWeight[i][userId] * act0;
            }
            code[i] = Math::Sigmoid(sum);
        }
    }
}

} //~ namespace CFAE

} //~ namespace longan
