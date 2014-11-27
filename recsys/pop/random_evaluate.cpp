/*
 * random_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_evaluate.h"
#include "random_predict.h"
#include "common/logging/logging.h"

namespace longan {

RandomEvaluate::RandomEvaluate(const std::string& ratingTrainFilepath,
    const std::string& configFilepath, const std::string& modelFilepath,
    const std::string& ratingTestFilepath, const std::string& resultFilepath) :
    BasicEvaluate(ratingTrainFilepath, configFilepath, modelFilepath,
            ratingTestFilepath, resultFilepath) {
}

RandomEvaluate::~RandomEvaluate() { }

void RandomEvaluate::CreatePredict() {
    Log::I("recsys", "RandomEvaluate::CreatePredict()");
    mPredict = new RandomPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
