/*
 * basic_convert.cpp
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#include "basic_convert.h"
#include "rating_list.h"
#include "common/logging/logging.h"

namespace longan {

BasicConvert::BasicConvert(const std::string& ratingTextFilepath, const std::string& ratingBinaryFilepath) :
    mRatingTextFilepath(ratingTextFilepath),
    mRatingBinaryFilepath(ratingBinaryFilepath) { }

BasicConvert::~BasicConvert() { }

void BasicConvert::Convert() {
    Log::I("recsys", "BasicConvert::Convert()");
    Log::I("recsys", "loading text rating file %s...", mRatingTextFilepath.c_str());
    RatingList rlist = RatingList::LoadFromTextFile(mRatingTextFilepath);
    Log::I("recsys", "rating file meta:\nNumRating=%d\nNumUser=%d\nNumItem=%d\nSparsity=%lf\n",
            rlist.NumRating(), rlist.NumUser(), rlist.NumItem(), rlist.Sparsity());
    Log::I("recsys", "writing binary rating file %s...", mRatingBinaryFilepath.c_str());
    RatingList::WriteToBinaryFile(rlist, mRatingBinaryFilepath);
}

} //~ namespace longan

