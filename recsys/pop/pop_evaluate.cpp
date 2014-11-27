/*
 * pop_evaluate.cpp
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#include "pop_evaluate.h"
#include "pop_predict.h"
#include "common/logging/logging.h"

namespace longan {

PopEvaluate::PopEvaluate(const std::string& ratingTrainFilepath, const std::string& configFilepath,
        const std::string& modelFilepath, const std::string& ratingTestFilepath,
        const std::string& resultFilepath) :
    BasicEvaluate(ratingTrainFilepath, configFilepath, modelFilepath, ratingTestFilepath, resultFilepath)
{ }

void PopEvaluate::CreatePredict() {
    Log::I("recsys", "PopEvaluate::CreatePredict()");
    mPredict = new PopPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
