/*
 * pop_train.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_TRAIN_H
#define RECSYS_POP_POP_TRAIN_H

#include "recsys/base/basic_train.h"
#include "recsys/base/rating_list.h"
#include "common/util/running_statistic.h"
#include <string>

namespace longan {
  
class PopTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
private:
    void SaveModel();
private:
    RatingList mRatingList;
    std::vector<RunningAverage<float>> mItemsAverage;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_TRAIN_H */
