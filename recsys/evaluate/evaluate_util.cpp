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
        mRankingListSize = option["rankingListSize"].asInt();
        assert(mRankingListSize > 0);
    } else {
        mRankingListSize = 0;
    }
    mMonitorProgress = option["monitorProgress"].asBool();
}

} //~ namespace longan
