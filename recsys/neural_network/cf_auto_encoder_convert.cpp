/*
 * cf_auto_encoder_convert.cpp
 * Created on: Mar 3, 2014
 * Author: chenguangyu
 */

#include "cf_auto_encoder_convert.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"

namespace longan {

CFAutoEncoderConvert::CFAutoEncoderConvert(
    const std::string& ratingTextFilepath, bool needNormalize,
    const std::string& ratingBinaryFilepath) :
    BasicConvert(ratingTextFilepath, ratingBinaryFilepath),
    mNeedNormalize(needNormalize) { }

void CFAutoEncoderConvert::Convert() {
    Log::I("recsys", "CFAutoEncoderConvert::Convert()");
    Log::I("recsys", "loading text rating file %s...", mRatingTextFilepath.c_str());
    RatingList rlist = RatingList::LoadFromTextFile(mRatingTextFilepath);
    if (mNeedNormalize) {
        Log::I("recsys", "normalize rating...");
        NormalizeRatingByDivideMax(&rlist);
    }
    Log::I("recsys", "rating file meta:\nNumRating=%d\nNumUser=%d\nNumItem=%d\nSparsity=%lf\n",
            rlist.NumRating(), rlist.NumUser(), rlist.NumItem(), rlist.Sparsity());
    Log::I("recsys", "writing binary rating file %s...", mRatingBinaryFilepath.c_str());
    RatingList::WriteToBinaryFile(rlist, mRatingBinaryFilepath);
}

} //~ namespace longan
