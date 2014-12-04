/*
 * user_based_evaluate.h
 * Created on: Nov 21, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_EVALUATE_H
#define RECSYS_NEIGHBOR_USER_BASED_EVALUATE_H

#include "recsys/base/basic_evaluate.h"

namespace longan {

class UserBasedEvaluate : public BasicEvaluate {
public:
    using BasicEvaluate::BasicEvaluate;
protected:
    virtual void CreatePredict() override;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_EVALUATE_H */
