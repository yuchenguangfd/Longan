/*
 * item_based_evaluate.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_evaluate.h"
#include "item_based_predict.h"
#include "common/logging/logging.h"

namespace longan {

void ItemBasedEvaluate::CreatePredict() {
    Log::I("recsys", "ItemBasedEvaluate::CreatePredict()");
    mPredict = new ItemBasedPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

} //~ namespace longan
