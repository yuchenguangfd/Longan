/*
 * pop_train.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_TRAIN_H
#define RECSYS_POP_POP_TRAIN_H

#include "recsys/base/basic_train.h"

namespace longan {
  
class PopTrain : public BasicTrain {
public:
    using BasicTrain::BasicTrain;
    virtual void Train() override;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_TRAIN_H */
