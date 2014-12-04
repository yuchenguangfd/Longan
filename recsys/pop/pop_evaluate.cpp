/*
 * pop_evaluate.cpp
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#include "pop_evaluate.h"
#include "pop_predict.h"
#include "common/logging/logging.h"

namespace longan {

void PopEvaluate::CreatePredict() {
    Log::I("recsys", "PopEvaluate::CreatePredict()");
    mPredict = new PopPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
