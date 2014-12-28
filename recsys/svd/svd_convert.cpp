/*
 * svdconvert.cpp
 *
 *  Created on: Dec 11, 2014
 *      Author: chenguangyu
 */

#include "svd_convert.h"
#include "recsys/base/rating_list.h"
#include "common/util/running_statistic.h"
#include "common/lang/binary_output_stream.h"

namespace longan {

void SVDConvert::Convert() {
    RatingList rlist = RatingList::LoadFromTextFile(mRatingTextFilepath);
    RunningAverage<double> raAvgRating;
    for (int i = 0; i < rlist.NumRating(); ++i) {
        raAvgRating.Add(rlist[i].Rating());
    }
    BinaryOutputStream bos(mRatingBinaryFilepath);
    bos << rlist.NumUser() << rlist.NumItem() << (int64)rlist.NumRating();
    bos << (float)raAvgRating.CurrentAverage();
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        bos << rr.UserId() << rr.ItemId() << rr.Rating();
    }
}

} //~ namespace longan
