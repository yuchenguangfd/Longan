/*
 * svd_convert.cpp
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#include "svd_convert.h"
#include "recsys/base/rating_list.h"
#include "common/util/running_statistic.h"
#include "common/lang/binary_output_stream.h"
#include "common/logging/logging.h"

namespace longan {

void SVDConvert::Convert() {
    Log::I("recsys", "SVDConvert::Convert()");
    Log::I("recsys", "loading text rating file %s...", mRatingTextFilepath.c_str());
    RatingList rlist = RatingList::LoadFromTextFile(mRatingTextFilepath);
    Log::I("recsys", "rating file meta:\nNumRating=%d\nNumUser=%d\nNumItem=%d\nSparsity=%lf\n",
                rlist.NumRating(), rlist.NumUser(), rlist.NumItem(), rlist.Sparsity());
    RunningAverage<double> runningAvgerage;
    for (int i = 0; i < rlist.NumRating(); ++i) {
        runningAvgerage.Add(rlist[i].Rating());
    }
    Log::I("recsys", "writing binary rating file %s...", mRatingBinaryFilepath.c_str());
    BinaryOutputStream bos(mRatingBinaryFilepath);
    bos << rlist.NumRating() << rlist.NumUser() << rlist.NumItem();
    bos << static_cast<float>(runningAvgerage.CurrentAverage());
    for (int i = 0; i < rlist.NumRating(); ++i) {
        const RatingRecord& rr = rlist[i];
        bos << rr.UserId() << rr.ItemId() << rr.Rating();
    }
}

} //~ namespace longan
