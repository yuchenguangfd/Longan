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
protected:
    virtual void CreateTrainOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void InitModel() override;
    virtual void ComputeModel() override;
    virtual void SaveModel() override;
    virtual void Cleanup() override;
private:
    const ItemBased::TrainOption *mTrainOption = nullptr;
    const ItemBased::Parameter *mParameter = nullptr;
    RatingMatItems *mTrainData = nullptr;
    ItemBased::Model *mModel = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
