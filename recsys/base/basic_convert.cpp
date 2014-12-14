/*
 * basic_convert.cpp
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#include "basic_convert.h"

namespace longan {

BasicConvert::BasicConvert(const std::string& ratingTextFilepath, const std::string& ratingBinaryFilepath) :
    mRatingTextFilepath(ratingTextFilepath),
    mRatingBinaryFilepath(ratingBinaryFilepath) { }

longan::BasicConvert::~BasicConvert() { }

} //~ namespace longan

