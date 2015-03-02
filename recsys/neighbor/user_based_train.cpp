/*
 * user_based_train.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_train.h"
#include "user_based_model_computation.h"
#include "common/logging/logging.h"

namespace longan {

void UserBasedTrain::Train() {
    Log::I("recsys", "UserBasedTrain::Train()");
    LoadConfig();
    CreateTrainOption();
    CreateParameter();
    LoadTrainData();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void UserBasedTrain::CreateTrainOption() {
    Log::I("recsys", "UserBasedTrain::CreateTrainOption()");
    mTrainOption = new UserBased::TrainOption(mConfig["trainOption"]);
}

void UserBasedTrain::CreateParameter() {
    Log::I("recsys", "UserBasedTrain::CreateParameter()");
    mParameter = new UserBased::Parameter(mConfig["parameter"]);
}

void UserBasedTrain::LoadTrainData() {
    Log::I("recsys", "UserBasedTrain::LoadTrainData()");
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
}

void UserBasedTrain::InitModel() {
    Log::I("recsys", "UserBasedTrain::InitModel()");
    mModel = new UserBased::ModelTrain(mParameter, mTrainData->NumUser());
}

void UserBasedTrain::ComputeModel() {
    Log::I("recsys", "UserBasedTrain::ComputeModel()");
    UserBased::ModelComputation *computationDelegate = nullptr;
    if (mTrainOption->Accelerate()) {
        computationDelegate = new UserBased::ModelComputationMT();
    } else {
        computationDelegate = new UserBased::ModelComputationST();
    }
    computationDelegate->ComputeModel(mTrainOption, mTrainData, mModel);
    delete computationDelegate;
}

void UserBasedTrain::SaveModel() {
    Log::I("recsys", "UserBasedTrain::SaveModel()");
    Log::I("recsys", "write model file = " + mModelFilepath);
    mModel->Save(mModelFilepath);
}

void UserBasedTrain::Cleanup() {
    Log::I("recsys", "UserBasedTrain::Cleanup()");
    delete mTrainOption;
    delete mParameter;
    delete mTrainData;
    delete mModel;
}

} //~ namespace longan
