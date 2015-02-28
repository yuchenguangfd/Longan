/*
 * svd_model_computation.h
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_MODEL_COMPUTATION_H
#define RECSYS_SVD_SVD_MODEL_COMPUTATION_H

#include "svd_util.h"
#include "svd_model.h"
#include "common/threading/pipelined_scheduler.h"
#include <vector>

namespace longan {

namespace SVD {

class ModelComputation {
public:
    virtual ~ModelComputation() { }
    virtual void ComputeModel(const TrainOption *option, const GriddedMatrix *trainData,
            const GriddedMatrix *validateData, ModelTrain *model) = 0;
protected:
    void SGDOneMatrix(int gridId);
    void ComputeLossInit();
    void ComputeLoss(double *trainDataLoss, double *trainRegLoss, double *validateDataLoss);
private:
    double ComputeDataLoss();
    double ComputeRegLoss();
    double ComputeValidateLoss();
protected:
    const TrainOption *mTrainOption = nullptr;
    const GriddedMatrix *mTrainData = nullptr;
    const GriddedMatrix *mValidateData = nullptr;
    ModelTrain *mModel = nullptr;
    std::vector<int> mNumRatingPerUser;
    std::vector<int> mNumRatingPerItem;
};

class ModelComputationST : public ModelComputation {
public:
    virtual void ComputeModel(const TrainOption *option, const GriddedMatrix *trainData,
            const GriddedMatrix *validateData, ModelTrain *model) override;
};

class ModelComputationMT : public ModelComputation, public PipelinedSchedulerClient {
public:
    virtual void ComputeModel(const TrainOption *option, const GriddedMatrix *trainData,
            const GriddedMatrix *validateData, ModelTrain *model) override;
    std::thread* CreateProducerThread() {
        return new std::thread(&ModelComputationMT::ProducerRun, this);
    }
    std::thread* CreateWorkerThread() {
        return new std::thread(&ModelComputationMT::WorkerRun, this);
    }
    std::thread* CreateConsumerThread() {
        return new std::thread(&ModelComputationMT::ConsumerRun, this);
    }
    std::thread* CreateMonitorThread() {
        return nullptr;
    }
private:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
private:
    enum TaskType {
        TaskTypeCompute, TaskTypeMonitor
    };
    struct Task {
        int type;
        int gridId;
        int iter;
    };
    PipelinedScheduler<Task> *mScheduler = nullptr;
    std::vector<bool> mGridRowBlocked;
    std::vector<bool> mGridColBlocked;
    std::vector<int> mGridIterationCount;
    std::mutex mGridBlockedMutex;
    std::mutex mMonitorMutex;
};

} //~ namespace SVD

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_MODEL_COMPUTATION_H */
