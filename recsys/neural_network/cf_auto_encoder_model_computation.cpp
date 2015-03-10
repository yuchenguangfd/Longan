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
    for (int ii = 0; ii < size; ++ii) {
        int itemId = data[ii].ItemId();
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
            if (i % 1000 == 0) Log::Console("recsys","SGD on sample %d/%d", i, mNumSample);
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
            if (i % 1000 == 0) Log::Console("recsys","SGD on sample %d/%d", i, mNumSample);
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
    for (int uid = 0; uid < trainData->NumUser(); ++uid) {
        Vector64F& code = model->mCodes[uid];
        const UserVec& uv = trainData->GetUserVector(uid);
        const ItemRating *data = uv.Data();
        int size = uv.Size();
        int numHiddenUnit = model->mParameter->NumLayerUnit(0);
        const Matrix64F& encodeWeight = model->mEncodeWeights[0];
        const Vector64F& encodeBias = model->mEncodeBiases[0];
        for (int i = 0; i < numHiddenUnit; ++i) {
            double sum = encodeBias[i];
            for (int j = 0; j < size; ++j) {
                int itemId = data[j].ItemId();
                float act0 = data[j].Rating();
                sum += encodeWeight[i][itemId] * act0;
            }
            code[i] = Math::Sigmoid(sum);
        }
        if (model->mParameter->NumLayer() > 1) {
            //TODO
        }
    }
}

void EncodeSamplesDelegateST::EncodeItem(const RatingMatItems *trainData, Model *model) {
    for (int iid = 0; iid < trainData->NumItem(); ++iid) {
        Vector64F& code = model->mCodes[iid];
        const ItemVec& iv = trainData->GetItemVector(iid);
        const UserRating *data = iv.Data();
        int size = iv.Size();
        int numHiddenUnit = model->mParameter->NumLayerUnit(0);
        const Matrix64F& encodeWeight = model->mEncodeWeights[0];
        const Vector64F& encodeBias = model->mEncodeBiases[0];
        for (int i = 0; i < numHiddenUnit; ++i) {
            double sum = encodeBias[i];
            for (int j = 0; j < size; ++j) {
                int userId = data[j].UserId();
                float act0 = data[j].Rating();
                sum += encodeWeight[i][userId] * act0;
            }
            code[i] = Math::Sigmoid(sum);
        }
        if (model->mParameter->NumLayer() > 1) {
            //TODO
        }
    }
}

} //~ namespace CFAE

//============================================================

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
