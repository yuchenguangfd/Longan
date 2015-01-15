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

namespace longan {

namespace SVD {

class ModelComputation {
public:
    virtual ~ModelComputation() { }
    virtual void ComputeModel(const TrainOption *trainOption, const GriddedMatrix *griddedMatrix, ModelTrain *model) = 0;
};

class SimpleModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(const TrainOption *trainOption, const GriddedMatrix *griddedMatrix, ModelTrain *model) override;
private:
    void SGDOnGrid(const Matrix& mat);
};

class FPSGDModelComputation : public ModelComputation, public PipelinedSchedulerClient {
public:
    virtual void ComputeModel(const TrainOption *trainOption, const GriddedMatrix *griddedMatrix, ModelTrain *model) override;
    std::thread* CreateProducerThread() {
        return new std::thread(&FPSGDModelComputation::ProducerRun, this);
    }
    std::thread* CreateWorkerThread() {
        return new std::thread(&FPSGDModelComputation::WorkerRun, this);
    }
    std::thread* CreateConsumerThread() {
        return new std::thread(&FPSGDModelComputation::ConsumerRun, this);
    }
    std::thread* CreateMonitorThread() {
        return new std::thread(&FPSGDModelComputation::MonitorRun, this);
    }
private:
    void Init();
    void CreateScheduler();
    void StartScheduler();
    void Cleanup();
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
private:
    const TrainOption *mTrainOption = nullptr;
    const GriddedMatrix *mGriddedMatrix = nullptr;
    ModelTrain *mModel = nullptr;
    struct Task {
       int gridId;
    };
    PipelinedScheduler<Task> *mScheduler = nullptr;
    std::vector<bool> mGridRowBlocked;
    std::vector<bool> mGridColBlocked;
    std::vector<int> mGridIterationCount;
    int mCurrentIteration = 0;
    std::mutex mGridBlockedMutex;
};

} //~ namespace SVD

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_MODEL_COMPUTATION_H */
