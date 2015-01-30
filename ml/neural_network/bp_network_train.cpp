/*
 * bp_network_train.cpp
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#include "bp_network_train.h"
#include "bp_network.h"
#include "common/logging/logging.h"
#include "common/config/json_config_helper.h"

namespace longan {

BpNetworkTrain::BpNetworkTrain(const std::string& dataTrainFilepath,
    const std::string& configFilepath, const std::string& modelFilepath) :
    mDataTrainFilepath(dataTrainFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath),
    mTrainData(nullptr),
    mModel(nullptr) { }

void BpNetworkTrain::Train() {
    LoadConfig();
    LoadTrainData();
    InitModel();
    TrainModel();
    SaveModel();
    Cleanup();
}

void BpNetworkTrain::LoadConfig() {
    Log::I("ml", "BpNetworkTrain::LoadConfig()");
    Log::I("ml", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void BpNetworkTrain::LoadTrainData() {
    Log::I("ml", "BpNetworkTrain::LoadData()");
    Log::I("ml", "training data file = " + mDataTrainFilepath);
    mTrainData = new SupervisedDatamodel(ClassificationDatamodel::LoadFromBinaryFile(mDataTrainFilepath));
}

void BpNetworkTrain::InitModel() {
    Log::I("ml", "BpNetworkTrain::InitModel()");
    BpNetworkArchitecture arch(mConfig["architecture"]);
    mModel = new BpNetwork(arch);
}

void BpNetworkTrain::TrainModel() {
    Log::I("ml", "BpNetworkTrain::TrainModel()");
    BpNetworkTrainOption option(mConfig["trainOption"]);
    mModel->Train(&option, mTrainData);
}

void BpNetworkTrain::SaveModel() {
    Log::I("ml", "BpNetworkTrain::SaveModel()");
    Log::I("ml", "model file = " + mModelFilepath);
    mModel->Save(mModelFilepath);
}

void BpNetworkTrain::Cleanup() {
    delete mTrainData;
    delete mModel;
}

} //~ namespace longan
