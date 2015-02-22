/*
 * evaluate_novelty_delegate.h
 * Created on: Feb 19, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_NOVELTY_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_NOVELTY_DELEGATE_H

#include "evaluate_util.h"
#include "common/threading/pipelined_scheduler.h"
#include "common/util/running_statistic.h"

namespace longan {

class BasicPredict;
class RatingList;
class RatingTrait;

class EvaluateNoveltyDelegate {
public:
    virtual ~EvaluateNoveltyDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingList *trainData,
            const EvaluateOption *option) = 0;
    double Novelty() const { return mNovelty; }
protected:
    const BasicPredict *mPredict = nullptr;
    const EvaluateOption *mOption = nullptr;
    const RatingList *mTrainData = nullptr;
    RatingTrait *mRatingTrait = nullptr;
    double mNovelty = 0.0;
};

class EvaluateNoveltyDelegateST : public EvaluateNoveltyDelegate {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *trainData,
            const EvaluateOption *option) override;
};

class EvaluateNoveltyDelegateMT : public EvaluateNoveltyDelegate, public PipelinedSchedulerClient {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *trainData,
            const EvaluateOption *option) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&EvaluateNoveltyDelegateMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&EvaluateNoveltyDelegateMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&EvaluateNoveltyDelegateMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return mOption->MonitorProgress() ?
                new std::thread(&EvaluateNoveltyDelegateMT::MonitorRun, this) :
                nullptr;
    }
private:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
private:
    struct Task {
        int userId;
        float userNovelty;
        Task(int uid) : userId(uid), userNovelty(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 128;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;
    int mTotoalTask = 0;
    int mProcessedTask = 0;
    RunningAverage<double> mRunningNovelty;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_NOVELTY_DELEGATE_H */
