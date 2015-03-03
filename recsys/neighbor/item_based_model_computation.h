/*
 * item_based_mdoel_computation.h
 * Created on: Nov 9, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H
#define RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H

#include "item_based_util.h"
#include "item_based_model.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_trait.h"
#include "common/lang/types.h"
#include "common/threading/pipelined_scheduler.h"

namespace longan {

namespace ItemBased {

class ModelComputation {
public:
    virtual ~ModelComputation() { };
    virtual void ComputeModel(const TrainOption *option, RatingMatItems *trainData,
            ModelTrain *model) = 0;
protected:
    void AdjustRating();
    float ComputeSimilarity(const ItemVec& iv1, const ItemVec& iv2) const;
    float ComputeCosineSimilarity(const ItemVec& iv1, const ItemVec& iv2) const;
    float ComputeBinaryCosineSimilarity(const ItemVec& iv1, const ItemVec& iv2) const;
    float ComputeBinaryJaccardSimilarity(const ItemVec& iv1, const ItemVec& iv2) const;
protected:
    const TrainOption *mTrainOption = nullptr;
    RatingMatItems *mTrainData = nullptr;
    ModelTrain *mModel = nullptr;
};

class ModelComputationST : public ModelComputation {
public:
    virtual void ComputeModel(const TrainOption *option, RatingMatItems *trainData,
            ModelTrain *model) override;
};

class ModelComputationMT : public ModelComputation, public PipelinedSchedulerClient {
public:
    virtual void ComputeModel(const TrainOption *option, RatingMatItems *trainData,
            ModelTrain *model) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&ModelComputationMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&ModelComputationMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&ModelComputationMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return mTrainOption->MonitorProgress() ?
                new std::thread(&ModelComputationMT::MonitorRun, this) :
                nullptr;
    }
protected:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
protected:
    struct Task {
        int iid1;
        int iid2;
        float sim;
    };
    static const int TASK_BUNDLE_SIZE = 16384;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;
    int64 mTotoalTask;
    int64 mProcessedTask;
};

class ModelComputationMTStaticSchedule : public ModelComputation {
public:
    virtual void ComputeModel(const TrainOption *option, RatingMatItems *trainData,
            ModelTrain *model) override;
private:
    void ThreadRun(int64 taskIdBegin, int64 taskIdEnd);
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H */
