/*
 * user_based_mdoel_computation.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H
#define RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H

#include "user_based_util.h"
#include "user_based_model.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_trait.h"
#include "common/lang/types.h"
#include "common/threading/pipelined_scheduler.h"

namespace longan {

namespace UserBased {

class ModelComputation {
public:
    virtual ~ModelComputation() { }
    virtual void ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
            Model *model) = 0;
protected:
    void AdjustRating();
    float ComputeSimilarity(const UserVec& uv1, const UserVec& uv2) const;
    float ComputeCosineSimilarity(const UserVec& uv1, const UserVec& uv2) const;
    float ComputeBinaryCosineSimilarity(const UserVec& iv1, const UserVec& iv2) const;
    float ComputeBinaryJaccardSimilarity(const UserVec& iv1, const UserVec& iv2) const;
protected:
    const TrainOption *mTrainOption = nullptr;
    RatingMatUsers *mTrainData = nullptr;
    Model *mModel = nullptr;
};

class ModelComputationST : public ModelComputation {
public:
    virtual void ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
            Model *model) override;
};

class ModelComputationMT : public ModelComputation, public PipelinedSchedulerClient {
public:
    virtual void ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
            Model *model) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&ModelComputationMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&ModelComputationMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread(){
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
        int uid1;
        int uid2;
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
    virtual void ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
            Model *model) override;
private:
    void ThreadRun(int64 taskIdBegin, int64 taskIdEnd);
};

} //~ namespace UserBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H */
