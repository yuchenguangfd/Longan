/*
 * svd_evaluate.h
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_EVALUATE_H
#define RECSYS_SVD_SVD_EVALUATE_H

#include "recsys/base/basic_evaluate.h"

namespace longan {

class SVDEvaluate : public BasicEvaluate {
public:
    using BasicEvaluate::BasicEvaluate;
protected:
    virtual void CreatePredict() override;
    virtual void LoadTestData() override;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_EVALUATE_H */
