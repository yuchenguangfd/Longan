/*
 * user_based_predict.h
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_USER_BASED_PREDICT_H

#include "user_based_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_matrix_as_users.h"

namespace longan {

class UserBasedPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
protected:
    void LoadRatings();
    void LoadModel();
protected:
    RatingMatrixAsItems<> *mRatingMatrixAsItems = nullptr;
    RatingMatrixAsUsers<> *mRatingMatrixAsUsers = nullptr;
    user_based::ModelPredict *mModel = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_PREDICT_H */
