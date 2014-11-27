/*
 * basic_train.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_BASIC_TRAIN_H
#define RECSYS_BASE_BASIC_TRAIN_H

#include <string>

namespace longan {

class BasicTrain {
public:
    BasicTrain(const std::string& trainRatingFilepath, const std::string& configFilepath, const std::string& modelFilepath);
    virtual ~BasicTrain();
    virtual void Train() = 0;
protected:
    const std::string mTrainRatingFilepath;
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
};

} //~ namespace longan

#endif /* RECSYS_BASE_BASIC_TRAIN_H */
