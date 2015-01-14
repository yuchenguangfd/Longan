/*
 * item_based_mdoel_computation.h
 * Created on: Nov 9, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H
#define RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H

#include "item_based_model.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "common/threading/pipelined_scheduler.h"
#include "common/lang/types.h"

namespace longan {

namespace ItemBased {

class ModelComputation {
public:
    ModelComputation() { };
    virtual ~ModelComputation() { };
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) = 0;
protected:
    float ComputeSimilarity(const ItemVector<>& firstItemVector, const ItemVector<>& secondItemVector);
};

class SimpleModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) override;
};

class StaticScheduledModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) override;
protected:
    void DoWork(int64 taskIdBegin, int64 taskIdEnd);
protected:
    RatingMatrixAsItems<> *mRatingMatrix;
    ModelTrain *mModel;
    std::vector<std::mutex*> mUpdateModelMutexs;
};

class DynamicScheduledModelComputation : public ModelComputation, public PipelinedSchedulerClient {
public:
    DynamicScheduledModelComputation(int numThread) : ModelComputation(), mNumThread(numThread) {
        assert(numThread > 0);
    }
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&DynamicScheduledModelComputation::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&DynamicScheduledModelComputation::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
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
        int firstItemId;
        int secondItemId;
        float similarity;
        Task(int iid1, int iid2) : firstItemId(iid1), secondItemId(iid2), similarity(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 16384;
    typedef std::vector<Task> TaskBundle;
    RatingMatrixAsItems<> *mRatingMatrix = nullptr;
    ModelTrain *mModel = nullptr;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;
    int mNumThread;
    double mCurrentProgress = 0.0;
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H */
