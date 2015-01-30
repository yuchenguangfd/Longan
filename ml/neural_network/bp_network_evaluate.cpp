/*
 * bp_network_evaluate.cpp
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#include "bp_network_evaluate.h"
#include "bp_network.h"
#include "common/config/json_config_helper.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"

namespace longan {

BpNetworkEvaluate::BpNetworkEvaluate(
    const std::string& dataTrainFilepath, const std::string& configFilepath,
    const std::string& modelFilepath, const std::string& dataTestFilepath,
    const std::string& resultFilepath) :
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath),
    mDataTestFilepath(dataTestFilepath),
    mResultFilepath(resultFilepath),
    mTestData(nullptr),
    mModel(nullptr),
    mAccuracy(0.0),
    mErrorRate(0.0) { }

void BpNetworkEvaluate::Evaluate() {
    LoadConfig();
    LoadTestData();
    LoadModel();
    EvaluateAccuracy();
    WriteResult();
    Cleanup();
}

void BpNetworkEvaluate::LoadConfig() {
    Log::I("ml", "BpNetworkEvaluate::LoadConfig()");
    Log::I("ml", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void BpNetworkEvaluate::LoadTestData() {
    Log::I("ml", "BpNetworkEvaluate::LoadTestData()");
    Log::I("ml", "testing data file = " + mDataTestFilepath);
    mTestData = new SupervisedDatamodel(ClassificationDatamodel::LoadFromBinaryFile(mDataTestFilepath));
}

void BpNetworkEvaluate::LoadModel() {
    Log::I("ml", "BpNetworkEvaluate::LoadModel()");
    Log::I("ml", "model file = " + mModelFilepath);
    BpNetworkArchitecture arch(mConfig["architecture"]);
    mModel = new BpNetwork(arch);
    mModel->Load(mModelFilepath);
}

void BpNetworkEvaluate::EvaluateAccuracy() {
    Log::I("ml", "BpNetworkEvaluate::EvaluateAccuracy()");
    int wrongLabelCount = 0;
    for (int i = 0; i < mTestData->NumSample(); ++i) {
        const Vector64F& feature = mTestData->Feature(i);
        const Vector64F& target = mTestData->Target(i);
        const Vector64F predTarget = mModel->Predict(feature);
        const double *pos1, *pos2;
        Math::MaxInRange(target.Begin(), target.End(), pos1);
        Math::MaxInRange(predTarget.Begin(), predTarget.End(), pos2);
        int trueLabel = pos1 - target.Begin();
        int predLabel = pos2 - predTarget.Begin();
        if (trueLabel != predLabel) {
            ++wrongLabelCount;
        }
    }
    mErrorRate = static_cast<double>(wrongLabelCount) / mTestData->NumSample();
    mAccuracy = 1.0 - mErrorRate;
}

void BpNetworkEvaluate::WriteResult() {
    Log::I("ml", "BpNetworkEvaluate::WriteResult()");
    Json::Value result;
    result["classificationResult"]["accuracy"] = mAccuracy;
    result["classificationResult"]["errorRate"] = mErrorRate;
    std::string output = result.toStyledString();
    Log::I("ml", "result = \n" + output);
    Log::I("ml", "writing result to file = " + mResultFilepath);
    FileUtil::SaveFileContent(mResultFilepath, output);
}

void BpNetworkEvaluate::Cleanup() {
    delete mModel;
    delete mTestData;
}

} //~ namespace longan
