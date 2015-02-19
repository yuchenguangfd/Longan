/*
 * evaluate_util.h
 * Created on: Feb 9, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_UTIL_H
#define RECSYS_EVALUATE_EVALUATE_UTIL_H

#include <json/json.h>

namespace longan {

class EvaluateOption {
public:
    EvaluateOption(const Json::Value& option);
    bool Accelerate() const { return mAccelerate; }
    int NumThread() const { return mNumThread; }
    bool EvaluateRating() const { return mEvaluateRating; }
    bool EvaluateRanking() const { return mEvaluateRanking; }
    bool EvaluateCoverage() const { return mEvaluateCoverage; }
    bool EvaluateDiversity() const { return mEvaluateDiversity; }
    bool EvaluateNovelty() const { return mEvaluateNovelty; }
    int RankingListSize() const { return mRankingListSize; }
    bool MonitorProgress() const { return mMonitorProgress; }
private:
    bool mAccelerate;
    int mNumThread;
    bool mEvaluateRating;
    bool mEvaluateRanking;
    bool mEvaluateCoverage;
    bool mEvaluateDiversity;
    bool mEvaluateNovelty;
    int  mRankingListSize;
    bool mMonitorProgress;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_UTIL_H */
