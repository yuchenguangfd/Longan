/*
 * basic_evaluate.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_BASIC_EVALUATE_H
#define RECSYS_BASE_BASIC_EVALUATE_H

#include "basic_predict.h"
#include "rating_list.h"
#include "recsys/evaluate/evaluate_util.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

class BasicEvaluate {
public:
    BasicEvaluate(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath, const std::string& ratingTestFilepath,
            const std::string& resultFilepath);
    virtual ~BasicEvaluate();
    virtual void Evaluate();
protected:
    virtual void LoadConfig() final;
    virtual void LoadTestRatings();
    virtual void CreatePredict() = 0;
    virtual void CreateEvaluateOption();
    virtual void EvaluateRating();
    virtual void EvaluateRanking();
    virtual void EvaluateCoverage();
    virtual void WriteResult();
    virtual void Cleanup();
protected:
    const std::string mRatingTrainFilepath;
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
    const std::string mRatingTestFilepath;
    const std::string mResultFilepath;
    Json::Value mConfig;
    RatingList *mTestRatingList = nullptr;
    BasicPredict *mPredict = nullptr;
    EvaluateOption *mEvaluateOption = nullptr;
    Json::Value mResult;
    DISALLOW_COPY_AND_ASSIGN(BasicEvaluate);
};

} //~ namespace longan

#endif /* RECSYS_BASE_BASIC_EVALUATE_H */
