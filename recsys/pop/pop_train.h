/*
 * pop_train.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_TRAIN_H
#define RECSYS_POP_POP_TRAIN_H

#include "recsys/base/rating_record.h"
#include "common/util/running_statistic.h"
#include <string>

namespace longan {
  
class PopTrain {
public:
    PopTrain(const std::string& ratingTrainFilePath, const std::string& modelFilePath);
    void Train();
private:
    void ReadRatingFile();
    void InitRunningAverage();
    void DestroyRunningAverage();
    void AddToRunningAverage(const RatingRecord& rating);
    void WriteModelFile();
private:
    const std::string mRatingTrainFilePath;
    const std::string mModelFilePath;
    int mNumRating;
    int mNumUser;
    int mNumItem;
    typedef RunningAverage<float> RunningRatingAverage;
    std::vector<RunningRatingAverage*> mItemsAverage;
    std::vector<RunningRatingAverage*> mUsersAverage;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_TRAIN_H */
