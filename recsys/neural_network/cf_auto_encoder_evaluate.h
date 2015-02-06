/*
 * cf_auto_encoder_evaluate.h
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_EVALUATE_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_EVALUATE_H

#include "recsys/base/basic_evaluate.h"

namespace longan {

class CFAutoEncoderEvaluate : public BasicEvaluate {
public:
    using BasicEvaluate::BasicEvaluate;
protected:
    virtual void CreatePredict() override;
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_EVALUATE_H */
