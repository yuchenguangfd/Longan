/*
 * evaluate_diversity_delegate.h
 * Created on: Feb 18, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_DIVERSITY_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_DIVERSITY_DELEGATE_H

#include "evaluate_util.h"
#include "common/threading/pipelined_scheduler.h"
#include "common/util/running_statistic.h"

namespace longan {

class BasicPredict;
class RatingList;

class EvaluateDiversityDelegate {
public:
    virtual ~EvaluateDiversityDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) = 0;
    double Diversity() const { return mDiversity; }
protected:
    const BasicPredict *mPredict = nullptr;
    const RatingList *mTestData = nullptr;
    const EvaluateOption *mOption = nullptr;
    double mDiversity = 0.0;
};

class EvaluateDiversityDelegateST : public EvaluateDiversityDelegate {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) override;
};

class EvaluateDiversityDelegateMT : public EvaluateDiversityDelegate, public PipelinedSchedulerClient {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&EvaluateDiversityDelegateMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&EvaluateDiversityDelegateMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&EvaluateDiversityDelegateMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return mOption->MonitorProgress() ?
               new std::thread(&EvaluateDiversityDelegateMT::MonitorRun, this) :
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
        float userDiversity;
        Task(int uid) : userId(uid), userDiversity(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 128;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;
    int mTotoalTask = 0;
    int mProcessedTask = 0;
    RunningAverage<double> mRunningDiversity;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_DIVERSITY_DELEGATE_H */
