/*
 * user_based_mdoel_computation.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H
#define RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H

#include "user_based_model.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/threading/pipelined_scheduler.h"
#include "common/lang/types.h"

namespace longan {

namespace UserBased {

class ModelComputation {
public:
    ModelComputation();
    virtual ~ModelComputation();
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) = 0;
protected:
    float ComputeSimilarity(const UserVector<>& firstUserVector, const UserVector<>& secondUserVector);
};

class SimpleModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) override;
};

class StaticScheduledModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) override;
protected:
    void DoWork(int64 taskIdBegin, int64 taskIdEnd);
protected:
    RatingMatrixAsUsers<> *mRatingMatrix;
    ModelTrain *mModel;
    std::vector<std::mutex*> mUpdateModelMutexs;
};

class DynamicScheduledModelComputation : public ModelComputation, public PipelinedSchedulerClient {
public:
    DynamicScheduledModelComputation(int numThread) : ModelComputation(), mNumThread(numThread) {
        assert(numThread > 0);
    }
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&DynamicScheduledModelComputation::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&DynamicScheduledModelComputation::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread(){
        return new std::thread(&DynamicScheduledModelComputation::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return new std::thread(&DynamicScheduledModelComputation::MonitorRun, this);
    }
protected:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
protected:
    struct Task {
        int firstUserId;
        int secondUserId;
        float similarity;
        Task(int uid1, int uid2) :
            firstUserId(uid1),
            secondUserId(uid2),
            similarity(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 16384;
    typedef std::vector<Task> TaskBundle;

    RatingMatrixAsUsers<> *mRatingMatrix = nullptr;
    ModelTrain *mModel = nullptr;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;
    int mNumThread;
    double mProgress = 0.0;
};

} //~ namespace UserBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H */
