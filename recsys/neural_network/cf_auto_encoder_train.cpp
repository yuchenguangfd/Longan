/*
 * cf_auto_encoder_train.cpp
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_train.h"
#include "cf_auto_encoder_model_computation.h"
#include "recsys/base/rating_list.h"
#include "common/logging/logging.h"

namespace longan {

void CFAutoEncoderTrain::CreateTrainOption() {
    Log::I("recsys", "CFAutoEncoderTrain::CreateTrainOption()");
    mTrainOption = new CFAE::TrainOption(mConfig["trainOption"]);
}

void CFAutoEncoderTrain::CreateParameter() {
    Log::I("recsys", "CFAutoEncoderTrain::CreateParameter()");
    mParameter = new CFAE::Parameter(mConfig["parameter"]);
}

void CFAutoEncoderTrain::LoadTrainData() {
    Log::I("recsys", "CFAutoEncoderTrain::LoadRatings()");
    Log::I("recsys", "train file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    if (mParameter->CodeType() == CFAE::Parameter::CodeType_User) {
        mTrainDataUsers = new RatingMatUsers();
        mTrainDataUsers->Init(rlist);
    } else if (mParameter->CodeType() == CFAE::Parameter::CodeType_Item) {
        mTrainDataItems = new RatingMatItems();
        mTrainDataItems->Init(rlist);
    }
}

void CFAutoEncoderTrain::LoadValidateData() {
    Log::I("recsys", "CFAutoEncoderTrain::LoadValidateData()");
    Log::I("recsys", "validate file = " + mRatingValidateFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    if (mParameter->CodeType() == CFAE::Parameter::CodeType_User) {
        mValidateDataUsers = new RatingMatUsers();
        mValidateDataUsers->Init(rlist);
    } else if (mParameter->CodeType() == CFAE::Parameter::CodeType_Item) {
        mValidateDataItems = new RatingMatItems();
        mValidateDataItems->Init(rlist);
    }
}

void CFAutoEncoderTrain::InitModel() {
    Log::I("recsys", "CFAutoEncoderTrain::InitModel()");
    if (mParameter->CodeType() == CFAE::Parameter::CodeType_User) {
        mModel = new CFAE::Model(mParameter, mTrainDataUsers->NumItem(), mTrainDataUsers->NumUser());
    } else if (mParameter->CodeType() == CFAE::Parameter::CodeType_Item) {
        mModel = new CFAE::Model(mParameter, mTrainDataItems->NumUser(), mTrainDataItems->NumItem());
    }
    if (mTrainOption->RandomInit()) {
        Log::I("recsys", "random init model.");
        mModel->RandomInit();
    } else {
        Log::I("recsys", "init from existing model file.");
        mModel->Load(mModelFilepath);
    }
}

void CFAutoEncoderTrain::ComputeModel() {
    Log::I("recsys", "CFAutoEncoderTrain::TrainModel()");
    CFAE::ModelComputation *delegate = new CFAE::ModelComputation() ;
    if (mParameter->CodeType() == CFAE::Parameter::CodeType_User) {
        delegate->ComputeModelUser(mTrainOption, mTrainDataUsers, mValidateDataUsers, mModel);
    } else if (mParameter->CodeType() == CFAE::Parameter::CodeType_Item) {
        delegate->ComputeModelItem(mTrainOption, mTrainDataItems, mValidateDataItems, mModel);
    }
    delete delegate;
}

void CFAutoEncoderTrain::SaveModel() {
    Log::I("recsys", "CFAutoEncoderTrain::SaveModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel->Save(mModelFilepath);
}

void CFAutoEncoderTrain::Cleanup() {
    delete mTrainOption;
    delete mParameter;
    delete mTrainDataUsers;
    delete mValidateDataUsers;
    delete mTrainDataItems;
    delete mValidateDataItems;
    delete mModel;
}

} //~ namespace longan
