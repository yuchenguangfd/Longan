/*
 * user_based_train.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_TRAIN_H
#define RECSYS_NEIGHBOR_USER_BASED_TRAIN_H

#include "user_based_model.h"
#include "user_based_model_computation.h"
#include "recsys/base/basic_train.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_trait.h"

namespace longan {

class UserBasedTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
protected:
    void LoadRatings();
    void AdjustRating();
    void AdjustRatingByMinusUserAverage();
    void AdjustRatingByMinusItemAverage();
    void InitModel();
    void ComputeModel();
    void SaveModel();
    void Cleanup();
protected:
    RatingMatrixAsUsers<> *mRatingMatrix = nullptr;
    RatingTrait *mRatingTrait = nullptr;
    user_based::ModelTrain *mModel = nullptr;
    user_based::ModelComputation *mModelComputationDelegate = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_TRAIN_H */
