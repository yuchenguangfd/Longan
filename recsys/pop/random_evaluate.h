/*
 * random_evaluate.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_RANDOM_EVALUATE_H
#define RECSYS_POP_RANDOM_EVALUATE_H

#include "recsys/base/basic_evaluate.h"

namespace longan {

class RandomEvaluate : public BasicEvaluate {
public:
    RandomEvaluate(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath, const std::string& ratingTestFilepath,
            const std::string& resultFilepath);
    ~RandomEvaluate();
protected:
    virtual void CreatePredict() override;
};

} //~ namespace longan

#endif /* RECSYS_POP_RANDOM_EVALUATE_H */
