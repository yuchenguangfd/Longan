/*
 * item_based_predict.h
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_ITEM_BASED_PREDICT_H

#include "item_based_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

class ItemBasedPredict : public BasicPredict {
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
    RatingMatrixAsUsers<> *mRatingMatrix;
    item_based::ModelPredict *mModel;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
