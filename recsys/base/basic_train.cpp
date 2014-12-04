/*
 * basic_train.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_train.h"
#include "common/system/file_util.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

BasicTrain::BasicTrain(const std::string& ratingTrainFilepath, const std::string& configFilepath, const std::string& modelFilepath) :
    mRatingTrainFilepath(ratingTrainFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath) { }

BasicTrain::~BasicTrain() { }

void BasicTrain::LoadConfig() {
    Log::I("recsys", "BasicTrain::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

} //~ namespace longan
