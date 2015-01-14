/*
 * evaluate_ranking_delegate.h
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_RANKING_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_RANKING_DELEGATE_H

#include "recsys/base/rating_matrix_as_users.h"
#include "common/threading/pipelined_scheduler.h"

namespace longan {

class BasicPredict;

class EvaluateRankingDelegate {
public:
    EvaluateRankingDelegate() : mPredict(nullptr), mTestRatingMatrix(nullptr),
        mRankingListSize(0), mPrecision(0.0), mRecall(0.0), mF1Score(0.0) { }
    virtual ~EvaluateRankingDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize) = 0;
    double Precision() const { return mPrecision; }
    double Recall() const { return mRecall; }
    double F1Score() const { return mF1Score; }
protected:
    void EvaluateOneUser(int userId, const ItemRating* truthDataBegin, int truthDataSize, double* precision, double* recall);
protected:
    const BasicPredict *mPredict;
    const RatingMatrixAsUsers<> *mTestRatingMatrix;
    int mRankingListSize;
    double mPrecision;
    double mRecall;
    double mF1Score;
};

class SimpleEvaluateRankingDelegate : public EvaluateRankingDelegate {
public:
    using EvaluateRankingDelegate::EvaluateRankingDelegate;
    virtual void Evaluate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize) override;
};

class DynamicScheduledEvaluateRankingDelegate : public EvaluateRankingDelegate, public PipelinedSchedulerClient {
public:
    using EvaluateRankingDelegate::EvaluateRankingDelegate;
    virtual void Evaluate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&DynamicScheduledEvaluateRankingDelegate::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&DynamicScheduledEvaluateRankingDelegate::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&DynamicScheduledEvaluateRankingDelegate::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return new std::thread(&DynamicScheduledEvaluateRankingDelegate::MonitorRun, this);
    }
private:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
    struct Task {
        int userId;
        double precision;
        double recall;
        Task(int uid): userId(uid), precision(0.0), recall(0.0) { }
    };
    static const int TASK_BUNDLE_SIZE = 512;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler;
    double mProgress;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_RANKING_DELEGATE_H */
