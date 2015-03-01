/*
 * item_based_train.cpp
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#include "item_based_train.h"
#include "item_based_model_computation.h"
#include "common/logging/logging.h"

namespace longan {

void ItemBasedTrain::Train() {
    Log::I("recsys", "ItemBasedTrain::Train()");
    LoadConfig();
    CreateTrainOption();
    CreateParameter();
    LoadTrainData();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void ItemBasedTrain::CreateTrainOption() {
    Log::I("recsys", "ItemBasedTrain::CreateTrainOption()");
    mTrainOption = new ItemBased::TrainOption(mConfig["trainOption"]);
}

void ItemBasedTrain::CreateParameter() {
    Log::I("recsys", "ItemBasedTrain::CreateParameter()");
    mParameter = new ItemBased::Parameter(mConfig["parameter"]);
}

void ItemBasedTrain::LoadTrainData() {
    Log::I("recsys", "ItemBasedTrain::LoadTrainData()");
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatItems();
    mTrainData->Init(rlist);
}

void ItemBasedTrain::InitModel() {
    Log::I("recsys", "ItemBasedTrain::InitModel()");
    mModel = new ItemBased::ModelTrain(mParameter, mTrainData->NumItem());
}

void ItemBasedTrain::ComputeModel() {
    Log::I("recsys", "ItemBasedTrain::ComputeModel()");
    ItemBased::ModelComputation *computationDelegate = nullptr;
    if (mTrainOption->Accelerate()) {
        computationDelegate = new ItemBased::ModelComputationMT();
    } else {
        computationDelegate = new ItemBased::ModelComputationST();
    }
    computationDelegate->ComputeModel(mTrainOption, mTrainData, mModel);
    delete computationDelegate;
}

void ItemBasedTrain::SaveModel() {
    Log::I("recsys", "ItemBasedTrain::SaveModel()");
    Log::I("recsys", "write model file = " + mModelFilepath);
    mModel->Save(mModelFilepath);
}

void ItemBasedTrain::Cleanup() {
    Log::I("recsys", "ItemBasedTrain::Clueanup()");
    delete mTrainOption;
    delete mParameter;
    delete mTrainData;
    delete mModel;
}

} //~ namespace longan
