/*
 * user_based_train.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_TRAIN_H
#define RECSYS_NEIGHBOR_USER_BASED_TRAIN_H

#include "user_based_model.h"
#include "user_based_model_computation.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_trait.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

class UserBasedTrain {
public:
    UserBasedTrain(const std::string& trainRatingFilepath, const std::string& configFilepath,
            const std::string& modelFilepath);
    ~UserBasedTrain();
    void Train();
protected:
    void LoadConfig();
    void LoadRatings();
    void AdjustRating();
    void AdjustRatingByMinusUserAverage();
    void AdjustRatingByMinusItemAverage();
    void InitModel();
    void ComputeModel();
    void SaveModel();
    void Cleanup();
protected:
    std::string mTrainRatingFilepath;
    std::string mConfigFilepath;
    std::string mModelFilepath;
    Json::Value mConfig;
    RatingMatrixAsUsers<> *mRatingMatrix;
    RatingTrait *mRatingTrait;
    user_based::ModelTrain *mModel;
    user_based::ModelComputation *mModelComputationDelegate;
    DISALLOW_COPY_AND_ASSIGN(UserBasedTrain);
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_TRAIN_H */
