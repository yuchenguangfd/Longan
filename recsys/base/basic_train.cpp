/*
 * basic_train.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_train.h"

namespace longan {

BasicTrain::BasicTrain(const std::string& trainRatingFilepath, const std::string& configFilepath, const std::string& modelFilepath) :
    mTrainRatingFilepath(trainRatingFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath) { }

BasicTrain::~BasicTrain() { }

} //~ namespace longan
