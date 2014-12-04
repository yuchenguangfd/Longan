/*
 * user_based_evaluate.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "user_based_evaluate.h"
#include "user_based_predict.h"
#include "common/logging/logging.h"

namespace longan {

void UserBasedEvaluate::CreatePredict() {
    Log::I("recsys", "UserBasedEvaluate::CreatePredict()");
    mPredict = new UserBasedPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
