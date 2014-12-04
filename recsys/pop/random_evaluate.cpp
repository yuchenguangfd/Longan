/*
 * random_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_evaluate.h"
#include "random_predict.h"
#include "common/logging/logging.h"

namespace longan {

void RandomEvaluate::CreatePredict() {
    Log::I("recsys", "RandomEvaluate::CreatePredict()");
    mPredict = new RandomPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
