/*
 * svd_train.cpp
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#include "svd_train.h"
#include "svd_model_computation.h"
#include "common/util/array_helper.h"
#include "common/logging/logging.h"

namespace longan {

void SVDTrain::CreateTrainOption() {
    Log::I("recsys", "SVDTrain::CreateTrainOption()");
    mTrainOption = new SVD::TrainOption(mConfig["trainOption"]);
}

void SVDTrain::CreateParameter() {
    Log::I("recsys", "SVDTrain::CreateParameter()");
    mParameter = new SVD::Parameter(mConfig["parameter"]);
}

void SVDTrain::LoadTrainData() {
    Log::I("recsys", "SVDTrain::LoadTrainData()");
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    SVD::Matrix meta = SVD::Matrix::LoadFromBinaryFile(mRatingTrainFilepath, true);
    GenerateIdMapping(meta.NumUser(), &mUserIdMapping);
    GenerateIdMapping(meta.NumItem(), &mItemIdMapping);
    mTrainData = new SVD::GriddedMatrix(SVD::GriddedMatrix::LoadFromBinaryFile(mRatingTrainFilepath,
            mTrainOption->NumUserBlock(), mTrainOption->NumItemBlock(),
            mUserIdMapping, mItemIdMapping));
}

void SVDTrain::LoadValidateData() {
    Log::I("recsys", "SVDTrain::LoadValidateData()");
    Log::I("recsys", "validate rating file = " + mRatingValidateFilepath);
    mValidateData = new SVD::GriddedMatrix(SVD::GriddedMatrix::LoadFromBinaryFile(mRatingValidateFilepath,
            mTrainOption->NumUserBlock(), mTrainOption->NumItemBlock(),
            mUserIdMapping, mItemIdMapping));
}

void SVDTrain::InitModel() {
    Log::I("recsys", "SVDTrain::InitModel()");
    mModel = new SVD::ModelTrain(mParameter, mTrainData->NumUser(), mTrainData->NumItem(), mTrainData->RatingAverage());
    if (mTrainOption->RandomInit()) {
        Log::I("recsys", "random init model");
        mModel->RandomInit();
    } else {
        Log::I("recsys", "init from existing model file = " + mModelFilepath);
        mModel->Load(mModelFilepath);
    }
}

void SVDTrain::GenerateIdMapping(int size, std::vector<int> *idMapping) {
    idMapping->resize(size);
    ArrayHelper::FillRange(idMapping->data(), idMapping->size());
    if (mTrainOption->UseRandomShuffle()) {
        ArrayHelper::RandomShuffle(idMapping->data(), idMapping->size());
    }
}

void SVDTrain::ComputeModel() {
    Log::I("recsys", "SVDTrain::ComputeModel()");
    SVD::ModelComputation *delegate = nullptr;
    if (mTrainOption->Accelerate()) {
        delegate = new SVD::ModelComputationMT();
    } else {
        delegate = new SVD::ModelComputationST();
    }
    delegate->ComputeModel(mTrainOption, mTrainData, mValidateData, mModel);
    delete delegate;
}

void SVDTrain::SaveModel() {
    Log::I("recsys", "SVDTrain::SaveModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    if (mTrainOption->UseRandomShuffle()) {
        InverseShuffleModel();
    }
    mModel->Save(mModelFilepath);
}

void SVDTrain::Cleanup() {
    delete mTrainOption;
    delete mParameter;
    delete mTrainData;
    delete mValidateData;
    delete mModel;
}

void SVDTrain::InverseShuffleModel() {
    auto getInverseIdMaping = [] (const std::vector<int> &mapping)->std::vector<int> {
        std::vector<int> invMapping(mapping.size());
        for (int i = 0; i < mapping.size(); i++) {
          invMapping[mapping[i]] = i;
        }
        return std::move(invMapping);
    };
    auto inverseShuffleFeatures = [] (std::vector<Vector32F>& features, const std::vector<int>& mapping) {
        assert(features.size() == mapping.size());
        std::vector<Vector<float>> temp(mapping.size());
        for (int i = 0; i < mapping.size(); ++i) {
            temp[i] = std::move(features[i]);
        }
        for(int i = 0; i < mapping.size(); ++i) {
            features[mapping[i]] = std::move(temp[i]);
        }
    };
    auto inverseShuffleBiases = [] (std::vector<float>& biases, const std::vector<int>& mapping){
        assert(biases.size() == mapping.size());
        std::vector<float> temp(mapping.size());
        std::copy(biases.begin(), biases.end(), temp.begin());
        for (int i = 0; i < mapping.size(); ++i) {
            biases[mapping[i]] = temp[i];
        }
    };
    const std::vector<int> uidInvMapping = getInverseIdMaping(mUserIdMapping);
    const std::vector<int> iidInvMapping = getInverseIdMaping(mItemIdMapping);
    inverseShuffleFeatures(mModel->mUserFeatures, uidInvMapping);
    inverseShuffleFeatures(mModel->mItemFeatures, iidInvMapping);
    if(mParameter->LambdaUserBias() >= 0.0f) {
        inverseShuffleBiases(mModel->mUserBiases, uidInvMapping);
    }
    if(mParameter->LambdaItemBias() >= 0.0f) {
        inverseShuffleBiases(mModel->mItemBiases, iidInvMapping);
    }
}

} //~ namespace longan
