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

void SVDTrain::Train() {
    LoadConfig();
    CreateTrainOption();
    CreateParameter();
    InitModel();
    LoadRatings();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void SVDTrain::CreateTrainOption() {
    Log::I("recsys", "SVDTrain::CreateTrainOption()");
    mTrainOption = new SVD::TrainOption(mConfig);
}

void SVDTrain::CreateParameter() {
    Log::I("recsys", "SVDTrain::CreateParameter()");
    mParameter = new SVD::Parameter(mConfig);
}

void SVDTrain::InitModel() {
    Log::I("recsys", "SVDTrain::InitModel()");
    SVD::Matrix metaInfo = SVD::Matrix::LoadMetaOnlyFromBinaryFile(mRatingTrainFilepath);
    mModel = new SVD::ModelTrain(*mParameter, metaInfo.NumUser(), metaInfo.NumItem(),
            metaInfo.RatingAverage());
    mModel->RandomInit();
}

void SVDTrain::LoadRatings() {
    Log::I("recsys", "SVDTrain::LoadRatings()");
    GenerateIdMapping(mModel->NumUser(), &mUserIdMapping);
    GenerateIdMapping(mModel->NumItem(), &mItemIdMapping);
    mGriddedMatrix = SVD::GriddedMatrix::LoadFromBinaryFile(mRatingTrainFilepath,
            mTrainOption->NumUserBlock(), mTrainOption->NumItemBlock(),
            mUserIdMapping, mItemIdMapping);
}

void SVDTrain::ComputeModel() {
    Log::I("recsys", "SVDTrain::ComputeModel()");
    mModelComputationDelegate = new SVD::FPSGDModelComputation();
    mModelComputationDelegate->ComputeModel(mTrainOption, &mGriddedMatrix, mModel);
}

void SVDTrain::SaveModel() {
    Log::I("recsys", "SVDTrain::SaveModel()");
    if (mTrainOption->UseRandomShuffle()) {
        InverseShuffleModel();
    }
    mModel->Save(mModelFilepath);
}

void SVDTrain::Cleanup() {
    delete mTrainOption;
    delete mParameter;
    delete mModel;
    delete mModelComputationDelegate;
}

void SVDTrain::GenerateIdMapping(int size, std::vector<int> *idMapping) {
    idMapping->resize(size);
    ArrayHelper::FillRange(idMapping->data(), idMapping->size());
    if (mTrainOption->UseRandomShuffle()) {
        ArrayHelper::RandomShuffle(idMapping->data(), idMapping->size());
    }
}

void SVDTrain::InverseShuffleModel() {
    auto getInverseIdMaping = [] (const std::vector<int> &mapping)->std::vector<int> {
        std::vector<int> invMapping(mapping.size());
        for (int i = 0; i < mapping.size(); i++) {
          invMapping[mapping[i]] = i;
        }
        return std::move(invMapping);
    };
    auto inverseShuffleFeatures = [] (std::vector<Vector<float>>& features, const std::vector<int>& mapping) {
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
