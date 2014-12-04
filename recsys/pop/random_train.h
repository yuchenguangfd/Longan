/*
 * random_train.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_RANDOM_TRAIN_H
#define RECSYS_POP_RANDOM_TRAIN_H

#include "recsys/base/basic_train.h"

namespace longan {

class RandomTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
};

} //~ namespace longan

#endif /* RECSYS_POP_RANDOM_TRAIN_H */
