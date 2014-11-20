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
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

class ItemBasedTrain {
public:
    ItemBasedTrain(const std::string& trainRatingFilepath, const std::string& configFilepath,
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
    void Cleanup();
protected:
    std::string mTrainRatingFilepath;
    std::string mConfigFilepath;
    std::string mModelFilepath;
    Json::Value mConfig;
    RatingMatrixAsItems<> *mRatingMatrix;
    RatingTrait *mRatingTrait;
    item_based::ModelTrain *mModel;
    item_based::ModelComputation *mModelComputationDelegate;
    DISALLOW_COPY_AND_ASSIGN(ItemBasedTrain);
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
