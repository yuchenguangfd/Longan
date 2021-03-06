/*
 * cf_auto_encoder_train.h
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_TRAIN_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_TRAIN_H

#include "cf_auto_encoder_model.h"
#include "recsys/base/basic_train.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_matrix_as_items.h"
#include <json/json.h>
#include <string>

namespace longan {

class CFAutoEncoderTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
protected:
    virtual void CreateTrainOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void LoadValidateData() override;
    virtual void InitModel() override;
    virtual void ComputeModel() override;
    virtual void SaveModel() override;
    virtual void Cleanup() override;
private:
    const CFAE::TrainOption *mTrainOption = nullptr;
    const CFAE::Parameter *mParameter = nullptr;
    RatingMatUsers *mTrainDataUsers = nullptr;
    RatingMatItems *mTrainDataItems = nullptr;
    RatingMatUsers *mValidateDataUsers = nullptr;
    RatingMatItems *mValidateDataItems = nullptr;
    CFAE::Model *mModel = nullptr;
};


} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_TRAIN_H */
