/*
 * svd_train.h
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_TRAIN_H
#define RECSYS_SVD_SVD_TRAIN_H

#include "svd_util.h"
#include "svd_model.h"
#include "svd_model_computation.h"
#include "recsys/base/basic_train.h"
#include <memory>
#include <chrono>
#include <vector>

namespace longan {

class SVDTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
private:
    void CreateTrainOption();
    void CreateParameter();
    void InitModel();
    void LoadRatings();
    void ComputeModel();
    void SaveModel();
    void Cleanup();
    void GenerateIdMapping(int size, std::vector<int> *idMapping);
    void InverseShuffleModel();
private:
    SVD::TrainOption *mTrainOption = nullptr;
    SVD::Parameter *mParameter = nullptr;
    SVD::ModelTrain *mModel = nullptr;
    SVD::ModelComputation *mModelComputationDelegate = nullptr;
    std::vector<int> mUserIdMapping;
    std::vector<int> mItemIdMapping;
    SVD::GriddedMatrix mGriddedMatrix;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_TRAIN_H */
