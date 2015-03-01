/*
 * item_based_train.h
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H
#define RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H

#include "item_based_util.h"
#include "item_based_model.h"
#include "recsys/base/basic_train.h"
#include "recsys/base/rating_matrix_as_items.h"

namespace longan {

class ItemBasedTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
protected:
    void CreateTrainOption();
    void CreateParameter();
    void LoadTrainData();
    void InitModel();
    void ComputeModel();
    void SaveModel();
    void Cleanup();
protected:
    const ItemBased::TrainOption *mTrainOption = nullptr;
    const ItemBased::Parameter *mParameter = nullptr;
    RatingMatItems *mTrainData = nullptr;
    ItemBased::ModelTrain *mModel = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
