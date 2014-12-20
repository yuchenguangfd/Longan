/*
 * item_based_train.h
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H
#define RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H

#include "item_based_model.h"
#include "item_based_model_computation.h"
#include "recsys/base/basic_train.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_trait.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

class ItemBasedTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
protected:
    void LoadRatings();
    void AdjustRating();
    void InitModel();
    void ComputeModel();
    void SaveModel();
    void Cleanup();
protected:
    RatingMatrixAsItems<> *mRatingMatrix = nullptr;
    RatingTrait *mRatingTrait = nullptr;
    ItemBased::ModelTrain *mModel = nullptr;
    ItemBased::ModelComputation *mModelComputationDelegate = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
