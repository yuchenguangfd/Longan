/*
 * basic_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_predict.h"
#include "common/config/json_config_helper.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

BasicPredict::BasicPredict(
    const std::string& ratingTrainFilepath,
    const std::string& configFilepath,
    const std::string& modelFilepath) :
    mRatingTrainFilepath(ratingTrainFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath) { }

BasicPredict::~BasicPredict() { }

void BasicPredict::LoadConfig() {
    Log::I("recsys", "BasicPredict::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
    Log::I("recsys", "config = \n" + mConfig.toStyledString());
}

float BasicPredict::ComputeItemSimilarity(int itemId1, int itemId2) const {
    return 0.0f;
}

} //~ namespace longan
