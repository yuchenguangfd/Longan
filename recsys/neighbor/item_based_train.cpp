/*
 * item_based_train.cpp
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#include "item_based_train.h"
#include "item_based_model_computation.h"
#include "common/logging/logging.h"

namespace longan {

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
    mModel = new ItemBased::Model(mParameter, mTrainData->NumItem());
}

void ItemBasedTrain::ComputeModel() {
    Log::I("recsys", "ItemBasedTrain::ComputeModel()");
    ItemBased::ModelComputation *delegate = nullptr;
    if (mTrainOption->Accelerate()) {
        delegate = new ItemBased::ModelComputationMT();
    } else {
        delegate = new ItemBased::ModelComputationST();
    }
    delegate->ComputeModel(mTrainOption, mTrainData, mModel);
    delete delegate;
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
