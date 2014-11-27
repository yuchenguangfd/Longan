/*
 * basic_train.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_train.h"

namespace longan {

BasicTrain::BasicTrain(const std::string& ratingTrainFilepath, const std::string& configFilepath, const std::string& modelFilepath) :
    mRatingTrainFilepath(ratingTrainFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath) { }

BasicTrain::~BasicTrain() { }

} //~ namespace longan
