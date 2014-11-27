/*
 * basic_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_predict.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"
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
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

} //~ namespace longan
