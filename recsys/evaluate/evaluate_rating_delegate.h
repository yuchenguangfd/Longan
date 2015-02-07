/*
 * evaluate_rating_delegate.h
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_RATING_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_RATING_DELEGATE_H

#include "common/threading/pipelined_scheduler.h"
#include "common/util/running_statistic.h"

namespace longan {

class BasicPredict;
class RatingList;

class EvaluateRatingDelegate {
public:
    EvaluateRatingDelegate() : mPredict(nullptr), mTestRatingList(nullptr), mMAE(0.0), mRMSE(0.0) { }
    virtual ~EvaluateRatingDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testRatingList) = 0;
    double MAE() const { return mMAE; }
    double RMSE() const { return mRMSE; }
protected:
    const BasicPredict *mPredict;
    const RatingList *mTestRatingList;
    double mMAE;
    double mRMSE;
};

class EvaluateRatingDelegateST : public EvaluateRatingDelegate {
public:
    using EvaluateRatingDelegate::EvaluateRatingDelegate;
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testRatingList) override;
};

class EvaluateRatingDelegateMT : public EvaluateRatingDelegate , public PipelinedSchedulerClient {
public:
    using EvaluateRatingDelegate::EvaluateRatingDelegate;
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testRatingList) override;
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
        return new std::thread(&EvaluateRatingDelegateMT::MonitorRun, this);
    }
private:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
    struct Task {
        int userId;
        int itemId;
        float trueRating;
        float predictedRating;
        Task(int uid, int iid, float rating): userId(uid), itemId(iid),
                trueRating(rating), predictedRating(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 4096;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler;
    double mProgress;
    RunningAverage<float> mMAERunningAvg;
    RunningAverage<float> mRMSERunningAvg;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_RATING_DELEGATE_H */
