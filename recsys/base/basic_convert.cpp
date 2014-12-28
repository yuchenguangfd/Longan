/*
 * basic_convert.cpp
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#include "basic_convert.h"
#include "rating_list.h"

namespace longan {

BasicConvert::BasicConvert(const std::string& ratingTextFilepath, const std::string& ratingBinaryFilepath) :
    mRatingTextFilepath(ratingTextFilepath),
    mRatingBinaryFilepath(ratingBinaryFilepath) { }

BasicConvert::~BasicConvert() { }

void BasicConvert::Convert() {
    RatingList rlist = RatingList::LoadFromTextFile(mRatingTextFilepath);
    RatingList::WriteToBinaryFile(rlist, mRatingBinaryFilepath);
}

} //~ namespace longan

