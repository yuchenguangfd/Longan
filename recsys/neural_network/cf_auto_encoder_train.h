/*
 * cf_auto_encoder_train.h
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_TRAIN_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_TRAIN_H

#include "recsys/base/basic_train.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_matrix_as_items.h"
#include <json/json.h>
#include <string>

namespace longan {

class CFAutoEncoder;

class CFAutoEncoderTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
private:
    void LoadRatings();
    void AdjustRatings();
    void InitModel();
    void TrainModel();
    void SaveModel();
    void Cleanup();
private:
    RatingMatrixAsUsers<> *mRatingMatrixAsUsers = nullptr;
    RatingMatrixAsItems<> *mRatingMatrixAsItems = nullptr;
//    RatingTrait *mRatingTrait = nullptr;
    CFAutoEncoder *mAutoEncoder = nullptr;
};


} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_TRAIN_H */
