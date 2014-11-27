/*
 * pop_evaluate.h
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_EVALUATE_H
#define RECSYS_POP_POP_EVALUATE_H

#include "recsys/base/basic_evaluate.h"

namespace longan {

class PopEvaluate : public BasicEvaluate {
public:
    PopEvaluate(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath, const std::string& ratingTestFilepath,
            const std::string& resultFilepath);
protected:
    void CreatePredict() override;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_EVALUATE_H */
