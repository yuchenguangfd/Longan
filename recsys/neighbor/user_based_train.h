/*
 * user_based_train.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_TRAIN_H
#define RECSYS_NEIGHBOR_USER_BASED_TRAIN_H

#include "user_based_util.h"
#include "user_based_model.h"
#include "recsys/base/basic_train.h"
#include "recsys/base/rating_matrix_as_users.h"

namespace longan {

class UserBasedTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
protected:
    virtual void CreateTrainOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void InitModel() override;
    virtual void ComputeModel() override;
    virtual void SaveModel() override;
    virtual void Cleanup() override;
private:
    const UserBased::TrainOption *mTrainOption = nullptr;
    const UserBased::Parameter *mParameter = nullptr;
    RatingMatUsers *mTrainData = nullptr;
    UserBased::Model *mModel = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_TRAIN_H */
