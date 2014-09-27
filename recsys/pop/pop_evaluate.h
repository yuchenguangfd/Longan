/*
 * pop_evaluate.h
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_EVALUATE_H
#define RECSYS_POP_POP_EVALUATE_H

#include "pop_predict.h"
#include "recsys/base/rating_record.h"
#include <json/json.h>
#include <string>
#include <vector>

namespace longan {

class PopEvaluate {
public:
    PopEvaluate(const std::string& modelFilePath, const std::string& ratingTrainFilePath,
            const std::string& ratingTestFilePath, const std::string& resultFilePath,
            const std::string& configFilePath);
    void Evaluate();
private:
    void ReadConfig();
    void ReadTestRatingData();
    void EvaluateRating();
    void EvaluateRanking();
    void WriteResult();
private:
    const std::string mModelFilePath;
    const std::string mRatingTrainFilePath;
    const std::string mRatingTestFilePath;
    const std::string mResultFilePath;
    const std::string mConfigFilePath;
    Json::Value mConfig;
    PopPredict* mPopPredict;
    int mNumUser;
    int mNumItem;
    int mNumTestRating;
    std::vector<RatingRecord> mTestRatings;
    double mMAE;
    double mRMSE;
    double mPrecision;
    double mRecall;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_EVALUATE_H */
