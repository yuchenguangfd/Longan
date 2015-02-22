/*
 * evaluate_coverage_delegate.h
 * Created on: Feb 12, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_COVERAGE_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_COVERAGE_DELEGATE_H

#include "evaluate_util.h"
#include "common/threading/pipelined_scheduler.h"

namespace longan {

class BasicPredict;
class RatingList;

class EvaluateCoverageDelegate {
public:
    virtual ~EvaluateCoverageDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) = 0;
    double Coverage() const { return mCoverage; }
    double Entropy() const { return mEntropy; }
    double GiniIndex() const { return mGiniIndex; }
protected:
    const BasicPredict *mPredict = nullptr;
    const RatingList *mTestData = nullptr;
    const EvaluateOption *mOption = nullptr;
    double mCoverage = 0.0;
    double mEntropy = 0.0;
    double mGiniIndex = 0.0;
};

class EvaluateCoverageDelegateST : public EvaluateCoverageDelegate {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) override;
};

class EvaluateCoverageDelegateMT : public EvaluateCoverageDelegate, public PipelinedSchedulerClient {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingList *testData,
            const EvaluateOption *option) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&EvaluateCoverageDelegateMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&EvaluateCoverageDelegateMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&EvaluateCoverageDelegateMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return mOption->MonitorProgress() ?
               new std::thread(&EvaluateCoverageDelegateMT::MonitorRun, this) :
               nullptr;
    }
private:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
private:
    struct Task {
        int userIdBegin;
        int userIdEnd;
        std::vector<int> itemFreqs;
    };
    static const int NUM_USER_PER_TASK = 128;
    PipelinedScheduler<Task> *mScheduler = nullptr;
    int mTotoalTask = 0;
    int mProcessedTask = 0;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_COVERAGE_DELEGATE_H */
