/*
 * evaluate_rating_delegate.h
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_RATING_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_RATING_DELEGATE_H

#include "evaluate_util.h"
#include "common/threading/pipelined_scheduler.h"
#include "common/util/running_statistic.h"

namespace longan {

class BasicPredict;
class RatingList;

class EvaluateRatingDelegate {
public:
    virtual ~EvaluateRatingDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) = 0;
    double MAE() const { return mMAE; }
    double RMSE() const { return mRMSE; }
protected:
    const BasicPredict *mPredict = nullptr;
    const RatingList *mTestData = nullptr;
    const EvaluateOption *mOption = nullptr;
    double mMAE = 0.0;
    double mRMSE = 0.0;
};

class EvaluateRatingDelegateST : public EvaluateRatingDelegate {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) override;
};

class EvaluateRatingDelegateMT : public EvaluateRatingDelegate , public PipelinedSchedulerClient {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&EvaluateRatingDelegateMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&EvaluateRatingDelegateMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&EvaluateRatingDelegateMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return mOption->MonitorProgress() ?
               new std::thread(&EvaluateRatingDelegateMT::MonitorRun, this) :
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
        int itemId;
        float trueRating;
        float predRating;
        Task(int uid, int iid, float rating): userId(uid), itemId(iid), trueRating(rating),
                predRating(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 1024;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;
    int mTotoalTask = 0;
    int mProcessedTask = 0;
    RunningAverage<double> mRunningMAE;
    RunningAverage<double> mRunningMSE;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_RATING_DELEGATE_H */
