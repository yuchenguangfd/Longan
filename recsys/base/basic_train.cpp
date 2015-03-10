/*
 * basic_train.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_train.h"
#include "common/config/json_config_helper.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

BasicTrain::BasicTrain(const std::string& ratingTrainFilepath,
        const std::string& configFilepath, const std::string& modelFilepath) :
    BasicTrain(ratingTrainFilepath, "", configFilepath, modelFilepath) { }

BasicTrain::BasicTrain(const std::string& ratingTrainFilepath,
        const std::string& ratingValidateFilepath,
        const std::string& configFilepath, const std::string& modelFilepath) :
    mRatingTrainFilepath(ratingTrainFilepath),
    mRatingValidateFilepath(ratingValidateFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath) { }

void BasicTrain::Train() {
    Log::I("recsys", "BasicTrain::Train()");
    LoadConfig();
    CreateTrainOption();
    CreateParameter();
    LoadTrainData();
    LoadValidateData();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void BasicTrain::LoadConfig() {
    Log::I("recsys", "BasicTrain::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
    Log::I("recsys", "config = \n" + mConfig.toStyledString());
}

void BasicTrain::CreateTrainOption() {
    Log::I("recsys", "BasicTrain::CreateTrainOption()");
}

void BasicTrain::CreateParameter() {
    Log::I("recsys", "BasicTrain::CreateParameter()");
}

void BasicTrain::LoadTrainData() {
    Log::I("recsys", "BasicTrain::LoadTrainData()");
}

void BasicTrain::LoadValidateData() {
    Log::I("recsys", "BasicTrain::LoadValidateData()");
}

void BasicTrain::InitModel() {
    Log::I("recsys", "BasicTrain::InitModel()");
}

void BasicTrain::ComputeModel() {
    Log::I("recsys", "BasicTrain::ComputeModel()");
}

void BasicTrain::SaveModel() {
    Log::I("recsys", "BasicTrain::SaveModel()");
}

void BasicTrain::Cleanup() {
    Log::I("recsys", "BasicTrain::Cleanup()");
}

} //~ namespace longan
