/*
 * random_train.cpp
 * Created on: Sep 27, 2014
 * Author: chenguangyu
 */

#include "random_train.h"
#include "common/logging/logging.h"

namespace longan {

RandomTrain::RandomTrain(const std::string& trainRatingFilepath, const std::string& configFilepath,
        const std::string& modelFilepath) :
    BasicTrain(trainRatingFilepath, configFilepath, modelFilepath) { }

RandomTrain::~RandomTrain() { }

void RandomTrain::Train() {
    Log::I("recsys", "RandomTrain::Train()");
    Log::I("recsys", "do nothing");
}

} //~ namespace longan
