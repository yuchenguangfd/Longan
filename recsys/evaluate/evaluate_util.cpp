/*
 * evaluate_util.cpp
 * Created on: Feb 9, 2015
 * Author: chenguangyu
 */

#include "evaluate_util.h"
#include "common/system/system_info.h"
#include <cassert>

namespace longan {

EvaluateOption::EvaluateOption(const Json::Value& option) {
    mAccelerate = option["accelerate"].asBool();
    if (mAccelerate) {
        mNumThread = option["numThread"].asInt();
        if (mNumThread <= 0) {
            mNumThread = SystemInfo::GetNumCPUCore();
        }
    } else {
        mNumThread = 1;
    }
    mEvaluateRating = option["evaluateRating"].asBool();
    mEvaluateRanking = option["evaluateRanking"].asBool();
    mEvaluateCoverage = option["evaluateCoverage"].asBool();
    mEvaluateDiversity = option["evaluateDiversity"].asBool();
    mEvaluateNovelty = option["evaluateNovelty"].asBool();
    if (mEvaluateRanking || mEvaluateCoverage || mEvaluateDiversity || mEvaluateNovelty) {
        int n = option["rankingListSizes"].size();
        assert(n > 0);
        mRankingListSizes.resize(n);
        for (int i = 0; i < n; ++i) {
            mRankingListSizes[i] = option["rankingListSizes"][i].asInt();
            assert(mRankingListSizes[i] > 0);
        }
        mCurrentRankingListSize = mRankingListSizes[0];
    }
    mMonitorProgress = option["monitorProgress"].asBool();
}

} //~ namespace longan
