/*
 * evaluate_util.h
 * Created on: Feb 9, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_UTIL_H
#define RECSYS_EVALUATE_EVALUATE_UTIL_H

#include <json/json.h>
#include <vector>

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
    const std::vector<int>& RankingListSizes() const { return mRankingListSizes; }
    bool MonitorProgress() const { return mMonitorProgress; }
    int CurrentRankingListSize() const { return mCurrentRankingListSize; }
    void SetCurrentRankingListSize(int size) { mCurrentRankingListSize = size; }
private:
    bool mAccelerate;
    int mNumThread;
    bool mEvaluateRating;
    bool mEvaluateRanking;
    bool mEvaluateCoverage;
    bool mEvaluateDiversity;
    bool mEvaluateNovelty;
    std::vector<int> mRankingListSizes;
    bool mMonitorProgress;

    int mCurrentRankingListSize;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_UTIL_H */
