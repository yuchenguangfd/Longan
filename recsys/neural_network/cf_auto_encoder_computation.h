/*
 * cf_auto_encoder_computation.h
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_COMPUTATION_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_COMPUTATION_H

#include "cf_auto_encoder.h"
#include "common/threading/pipelined_scheduler.h"
#include <vector>

namespace longan {

class CFAutoEncoderComputation {
public:
    virtual ~CFAutoEncoderComputation();
    virtual void Train(CFAutoEncoder *autoEncoder, const CFAE::TrainOption *trainOption,
            const RatingMatrixAsUsers<> *data) = 0;
protected:
    void RandomInit();
protected:
    const CFAE::TrainOption *mTrainOption;
    const RatingMatrixAsUsers<> *mData;
    CFAutoEncoder *mAutoEncoder;
};

class CFAutoEncoderComputationST : public CFAutoEncoderComputation {
public:
    virtual void Train(CFAutoEncoder *autoEncoder, const CFAE::TrainOption *trainOption,
                const RatingMatrixAsUsers<> *data) override;
private:
    void InitResource();
    void Forward();
    void ForwardInputToHidden();
    void ForwardHiddenToOutput();
    void Backward();
    void BackwardOutputToHidden();
    void BackwardHiddenToInput();
    void UpdateNetwork();
    void UpdateNetworkInputToHidden();
    void UpdateNetworkHiddenToOutput();
    void ComputeTotalLoss();
private:
    Vector64F mActivation1;
    Vector64F mActivation2;
    Vector64F mDelta1;
    Vector64F mDelta2;
    std::vector<double> mSampleLosses;
    int mCurrentSampleId;
    double mDataLoss;
    double mRegLoss;
    double mTotalLoss;
};

class CFAutoEncoderComputationMT : public CFAutoEncoderComputationST, public PipelinedSchedulerClient {
public:
    virtual void Train(CFAutoEncoder *autoEncoder, const CFAE::TrainOption *trainOption,
                   const RatingMatrixAsUsers<> *data) override;
    virtual std::thread* CreateProducerThread() override {
        return new std::thread(&CFAutoEncoderComputationMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() override {
        return new std::thread(&CFAutoEncoderComputationMT::WorkerRun, this, mThreadId++);
    }
    virtual std::thread* CreateConsumerThread() override {
        return new std::thread(&CFAutoEncoderComputationMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() override {
        return nullptr;
    }
private:
    void ProducerRun() {}
    void WorkerRun(int threadId) {}
    void ConsumerRun() {}
    void InitResource() {}
//    void ProcessTask(int threadId, int sampleId, double *loss);
//    void Forward(int threadId);
//    void ForwardOneLayer(int inputUnit, int outputUnit, const double *inputAct,
//            const double *weight, const double *bias, double *outputAct);
//    void Backward(int threadId, double *loss);
//    void BackwardOutputLayer(int targetUnit, const double *inputAct, const double *targetAct,
//            double *outputDelta, double *loss);
//    void BackwardOneLayer(int inputUnit, int outputUnit, const double *weight,
//            const double *inputDelta, const double *inputAct, double *outputDelta);
//    void ComputeGradient(int threadId);
//    void ComputeGradientOneLayer(int inputUnit, int outputUnit, const double *delta,
//            const double *act, double *weightGrad, double *biasGrad);
//    void AdjustNetwork(int threadId);
//    void AdjustNetworkOneLayer(int inputUnit, int outputUnit, const double *weightGrad,
//            const double *biasGrad, double *weight, double *bias);
//    void ComputeTotalLoss();
//private:
//    enum TaskType {
//        TaskTypeCompute, TaskTypeMonitor
//    };
//    struct Task {
//        int type;
//        int sampleId;
//        int iter;
//        double sampleLoss;
//    };
//    static const int TASK_BUNDLE_SIZE = 256;
//    typedef std::vector<Task> TaskBundle;
//    PipelinedScheduler<TaskBundle> *mScheduler;
    int mThreadId;
//
//    std::vector<Vector64F> mTarget;
//    std::vector<std::vector<Vector64F>> mActivations;
//    std::vector<std::vector<Vector64F>> mDeltas;
//    std::vector<std::vector<Matrix64F>> mWeightGradients;
//    std::vector<std::vector<Vector64F>> mBiasGradients;
//    std::vector<double> mSampleLosses;
//    double mDataLoss;
//    double mRegLoss;
//    double mTotalLoss;
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_COMPUTATION_H */
