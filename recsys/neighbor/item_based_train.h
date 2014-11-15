/*
 * item_based_train.h
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H
#define RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H

#include "item_based_model.h"
#include "item_based_model_computation.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_trait.h"
#include <json/json.h>
#include <string>

namespace longan {

class ItemBasedTrain {
public:
    ItemBasedTrain(const std::string& trainRatingFilepath, const std::string& trainConfigFilepath,
            const std::string& modelFilepath);
    ~ItemBasedTrain();
    void Train();
protected:
    void LoadConfig();
    void LoadRatings();
    void AdjustRating();
    void AdjustRatingByMinusItemAverage();
    void AdjustRatingByMinusUserAverage();
    void InitModel();
    void ComputeModel();
    void SaveModel();
protected:
    std::string mTrainRatingFilepath;
    std::string mTrainConfigFilepath;
    std::string mModelFilepath;
    Json::Value mTrainConfig;
    RatingMatrixAsItems<> *mRatingMatrix;
    RatingTrait *mRatingTrait;
    item_based::ModelTrain *mModel;
    item_based::ModelComputation *mModelComputationDelegate;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
