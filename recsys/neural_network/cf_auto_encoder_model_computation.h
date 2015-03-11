/*
 * cf_auto_encoder_computation.h
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_COMPUTATION_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_COMPUTATION_H

#include "cf_auto_encoder_model.h"
#include "common/threading/pipelined_scheduler.h"

namespace longan {

namespace CFAE {

class ModelComputation {
public:
    void ComputeModelUser(const TrainOption *option, const RatingMatUsers *trainData,
            const RatingMatUsers *validateData, Model *model);
    void ComputeModelItem(const TrainOption *option, const RatingMatItems *trainData,
            const RatingMatItems *validateData, Model *model);
private:
    void TrainSparseLayerUser();
    void TrainSparseLayerItem();
    void EncodeSamplesUser();
    void EncodeSamplesItem();
protected:
    const TrainOption *mTrainOption = nullptr;
    const RatingMatUsers *mTrainDataUsers = nullptr;
    const RatingMatUsers *mValidateDataUsers = nullptr;
    const RatingMatItems *mTrainDataItems = nullptr;
    const RatingMatItems *mValidateDataItems = nullptr;
    Model *mModel = nullptr;
};

class TrainSparseLayerDelegate {
public:
    virtual ~TrainSparseLayerDelegate() { }
    virtual void TrainSparseLayerUser(const TrainOption *option, const RatingMatUsers *trainData,
            const RatingMatUsers *validateData, Model *model) = 0;
    virtual void TrainSparseLayerItem(const TrainOption *option, const RatingMatItems *trainData,
            const RatingMatItems *validateData, Model *model) = 0;
protected:
    void ForwardUser(int sampleId, Vector64F& activation1, Vector64F& activation2);
    void ForwardItem(int sampleId, Vector64F& activation1, Vector64F& activation2);
    void BackwardUser(int sampleId, const Vector64F& activation1, const Vector64F& activation2,
            Vector64F& delta1, Vector64F& delta2);
    void BackwardItem(int sampleId, const Vector64F& activation1, const Vector64F& activation2,
            Vector64F& delta1, Vector64F& delta2);
    void UpdateUser(int sampleId, const Vector64F& activation1, const Vector64F& delta1,
            const Vector64F& delta2);
    void UpdateItem(int sampleId, const Vector64F& activation1, const Vector64F& delta1,
            const Vector64F& delta2);
    double ComputeDataLoss();
    double ComputeRegLoss();
    double ComputeValidateLoss();
    double ComputeValidateLossUser();
    double ComputeValidateLossItem();
protected:
    const TrainOption *mTrainOption = nullptr;
    const RatingMatUsers *mTrainDataUsers = nullptr;
    const RatingMatUsers *mValidateDataUsers = nullptr;
    const RatingMatItems *mTrainDataItems = nullptr;
    const RatingMatItems *mValidateDataItems = nullptr;
    Model *mModel = nullptr;
    int mNumInputUnit;
    int mNumHiddenUnit;
    int mNumSample;
    std::vector<double> mSampleLoss;
};

class TrainSparseLayerDelegateST : public TrainSparseLayerDelegate {
public:
    virtual void TrainSparseLayerUser(const TrainOption *option, const RatingMatUsers *trainData,
            const RatingMatUsers *validateData, Model *model) override;
    virtual void TrainSparseLayerItem(const TrainOption *option, const RatingMatItems *trainData,
            const RatingMatItems *validateData, Model *model) override;
private:
    void InitResource();
private:
    Vector64F mActivation1;
    Vector64F mActivation2;
    Vector64F mDelta1;
    Vector64F mDelta2;
};

class TrainSparseLayerDelegateMT : public TrainSparseLayerDelegate, public PipelinedSchedulerClient {
public:
    virtual void TrainSparseLayerUser(const TrainOption *option, const RatingMatUsers *trainData,
            const RatingMatUsers *validateData, Model *model) override;
    virtual void TrainSparseLayerItem(const TrainOption *option, const RatingMatItems *trainData,
            const RatingMatItems *validateData, Model *model) override;
    virtual std::thread* CreateProducerThread() override {
        return new std::thread(&TrainSparseLayerDelegateMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() override {
        return new std::thread(&TrainSparseLayerDelegateMT::WorkerRun, this, mThreadId++);
    }
    virtual std::thread* CreateConsumerThread() override {
        return new std::thread(&TrainSparseLayerDelegateMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() override {
        return nullptr;
    }
private:
    void InitResource();
    void ProducerRun();
    void WorkerRun(int threadId);
    void ConsumerRun();
    void SGDOnSample(int threadId, int sampleId);
private:
    int mThreadId;
    std::vector<Vector64F> mActivation1;
    std::vector<Vector64F> mActivation2;
    std::vector<Vector64F> mDelta1;
    std::vector<Vector64F> mDelta2;
    enum TaskType {
        TaskTypeCompute, TaskTypeMonitor
    };
    struct Task {
        int type;
        int sampleId;
        int iter;
    };
    static const int TASK_BUNDLE_SIZE = 256;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler;
    std::mutex mMonitorIterationMutex;
};

class EncodeSamplesDelegateST {
public:
    void EncodeUser(const RatingMatUsers *trainData, Model *model);
    void EncodeItem(const RatingMatItems *trainData, Model *model);
};

} //~ namespace CFAE

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_COMPUTATION_H */
