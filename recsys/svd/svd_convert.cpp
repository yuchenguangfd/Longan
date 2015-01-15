/*
 * svd_convert.cpp
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#include "svd_convert.h"
#include "recsys/base/rating_list.h"
#include "common/util/running_statistic.h"
#include "common/lang/binary_output_stream.h"

namespace longan {

void SVDConvert::Convert() {
    RatingList rlist = RatingList::LoadFromTextFile(mRatingTextFilepath);
    RunningAverage<double> runningAvgerage;
    for (int i = 0; i < rlist.NumRating(); ++i) {
        runningAvgerage.Add(rlist[i].Rating());
    }
    BinaryOutputStream bos(mRatingBinaryFilepath);
    bos << rlist.NumRating() << rlist.NumUser() << rlist.NumItem();
    bos << static_cast<float>(runningAvgerage.CurrentAverage());
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        bos << rr.UserId() << rr.ItemId() << rr.Rating();
    }
}

} //~ namespace longan
