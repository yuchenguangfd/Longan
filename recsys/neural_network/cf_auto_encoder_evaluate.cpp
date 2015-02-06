/*
 * cf_auto_encoder_evaluate.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_evaluate.h"
#include "cf_auto_encoder_predict.h"
#include "common/logging/logging.h"

namespace longan {

void CFAutoEncoderEvaluate::CreatePredict() {
    Log::I("recsys", "CFAutoEncoderEvaluate::CreatePredict()");
    mPredict = new CFAutoEncoderPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
