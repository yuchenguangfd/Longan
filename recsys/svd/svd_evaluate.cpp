/*
 * svd_evaluate.cpp
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#include "svd_evaluate.h"
#include "svd_predict.h"
#include "common/logging/logging.h"

namespace longan {

void SVDEvaluate::CreatePredict() {
    Log::I("recsys", "SVDEvaluate::CreatePredict()");
    mPredict = new SVDPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
