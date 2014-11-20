/*
 * item_based_evaluate.h
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_EVALUATE_H
#define RECSYS_NEIGHBOR_ITEM_BASED_EVALUATE_H

#include "item_based_predict.h"
#include "recsys/base/rating_list.h"
#include <json/json.h>
#include <string>

namespace longan {

class ItemBasedEvaluate {
public:
    ItemBasedEvaluate(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath, const std::string& ratingTestFilepath,
            const std::string& resultFilepath);
    void Evaluate();
private:
    void LoadConfig();
    void LoadTestRatings();
    void CreatePredict();
    void EvaluateRating();
    void EvaluateRanking();
    void WriteResult();
    void Cleanup();
private:
    std::string mRatingTrainFilepath;
    std::string mConfigFilepath;
    std::string mModelFilepath;
    std::string mRatingTestFilepath;
    std::string mResultFilepath;
    Json::Value mConfig;
    ItemBasedPredict *mPredict;
    RatingList mTestRatingList;

    double mMAE;
    double mRMSE;
    double mPrecision;
    double mRecall;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_EVALUATE_H */