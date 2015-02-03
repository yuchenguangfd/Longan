/*
 * bp_network_computation.h
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#ifndef ML_NEURAL_NETWORK_BP_NETWORK_COMPUTATION_H
#define ML_NEURAL_NETWORK_BP_NETWORK_COMPUTATION_H

#include "common/math/vector.h"
#include "common/math/matrix.h"
#include "common/threading/pipelined_scheduler.h"
#include <vector>

namespace longan {

class BpNetwork;
class BpNetworkTrainOption;
class SupervisedDatamodel;

class BpNetworkComputation {
public:
    virtual ~BpNetworkComputation();
    virtual void Train(BpNetwork *bpNetwork, const BpNetworkTrainOption *trainOption,
            const SupervisedDatamodel *datamodel) = 0;
protected:
    void RandomInit();
protected:
    const BpNetworkTrainOption *mTrainOption;
    const SupervisedDatamodel *mDatamodel;
    BpNetwork *mNetwork;
};

class BpNetworkComputationSimple : public BpNetworkComputation {
public:
    virtual void Train(BpNetwork *network, const BpNetworkTrainOption *trainOption,
              const SupervisedDatamodel *datamodel) override;
private:
    void InitResource();
    void Forward();
    void Backward();
    void ComputeGradient();
    void AdjustNetwork();
    void ComputeTotalLoss();
private:
    Vector64F mTarget;
    std::vector<Vector64F> mActivations;
    std::vector<Vector64F> mDeltas;
    std::vector<Matrix64F> mWeightGradients;
    std::vector<Vector64F> mBiasGradients;
    std::vector<double> mSampleLosses;
    int mCurrentSampleId;
    double mDataLoss;
    double mRegLoss;
    double mTotalLoss;
};

class BpNetworkComputationOpenMP : public BpNetworkComputation {
public:
    virtual void Train(BpNetwork *network, const BpNetworkTrainOption *trainOption,
              const SupervisedDatamodel *datamodel) override;
private:
    void InitResource();
    void Forward();
    void ForwardOneLayer(int inputUnit, int outputUnit, const double *inputAct,
            const double *weight, const double *bias, double *outputAct);
    void Backward();
    void BackwardOutputLayer(int targetUnit, const double *inputAct, const double *targetAct,
            double *outputDelta);
    void BackwardOneLayer(int inputUnit, int outputUnit, const double *weight,
            const double *inputDelta, const double *inputAct, double *outputDelta);
    void ComputeGradient();
    void ComputeGradientOneLayer(int inputUnit, int outputUnit, const double *delta,
            const double *act, double *weightGrad, double *biasGrad);
    void AdjustNetwork();
    void AdjustNetworkOneLayer(int inputUnit, int outputUnit, const double *weightGrad,
            const double *biasGrad, double *weight, double *bias);
    void ComputeTotalLoss();
private:
    Vector64F mTarget;
    std::vector<Vector64F> mActivations;
    std::vector<Vector64F> mDeltas;
    std::vector<Matrix64F> mWeightGradients;
    std::vector<Vector64F> mBiasGradients;
    std::vector<double> mSampleLosses;
    int mCurrentSampleId;
    double mDataLoss;
    double mRegLoss;
    double mTotalLoss;
};

class BpNetworkComputationMT : public BpNetworkComputation, public PipelinedSchedulerClient {
public:
    virtual void Train(BpNetwork *network, const BpNetworkTrainOption *trainOption,
              const SupervisedDatamodel *datamodel) override;
    virtual std::thread* CreateProducerThread() override {
        return new std::thread(&BpNetworkComputationMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() override {
        return new std::thread(&BpNetworkComputationMT::WorkerRun, this, mThreadId++);
    }
    virtual std::thread* CreateConsumerThread() override {
        return new std::thread(&BpNetworkComputationMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() override {
        return nullptr;
    }
private:
    void ProducerRun();
    void WorkerRun(int threadId);
    void ConsumerRun();
    void InitResource();
    void ProcessTask(int threadId, int sampleId, double *loss);
    void Forward(int threadId);
    void ForwardOneLayer(int inputUnit, int outputUnit, const double *inputAct,
            const double *weight, const double *bias, double *outputAct);
    void Backward(int threadId, double *loss);
    void BackwardOutputLayer(int targetUnit, const double *inputAct, const double *targetAct,
            double *outputDelta, double *loss);
    void BackwardOneLayer(int inputUnit, int outputUnit, const double *weight,
            const double *inputDelta, const double *inputAct, double *outputDelta);
    void ComputeGradient(int threadId);
    void ComputeGradientOneLayer(int inputUnit, int outputUnit, const double *delta,
            const double *act, double *weightGrad, double *biasGrad);
    void AdjustNetwork(int threadId);
    void AdjustNetworkOneLayer(int inputUnit, int outputUnit, const double *weightGrad,
            const double *biasGrad, double *weight, double *bias);
    void ComputeTotalLoss();
private:
    enum TaskType {
        TaskTypeCompute, TaskTypeMonitor
    };
    struct Task {
        int type;
        int sampleId;
        int iter;
        double sampleLoss;
    };
    static const int TASK_BUNDLE_SIZE = 256;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler;
    int mThreadId;

    std::vector<Vector64F> mTarget;
    std::vector<std::vector<Vector64F>> mActivations;
    std::vector<std::vector<Vector64F>> mDeltas;
    std::vector<std::vector<Matrix64F>> mWeightGradients;
    std::vector<std::vector<Vector64F>> mBiasGradients;
    std::vector<double> mSampleLosses;
    double mDataLoss;
    double mRegLoss;
    double mTotalLoss;
};

} //~ namespace longan

#endif /* ML_NEURAL_NETWORK_BP_NETWORK_COMPUTATION_H */
