/*
 * characterdataset.h
 * Created on: Mar 11, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_UTIL_CHARACTER_DATASET_H
#define RECSYS_UTIL_CHARACTER_DATASET_H

#include "recsys/base/rating_list.h"
#include "recsys/base/rating_trait.h"
#include <json/json.h>
#include <string>

namespace longan {

class CharacterDataset {
public:
    CharacterDataset(const std::string& ratingFilepath, const std::string& resultFilepath);
    void Character();
private:
    void ReportMeta();
    void ReportUserActivity();
    void ReportItemPopularity();
private:
    const std::string mRatingFilepath;
    const std::string mResultFilepath;
    RatingList mRatingData;
    RatingTrait mRatingTrait;
    Json::Value mResult;
};

} //~ namespace longan

#endif /* RECSYS_UTIL_CHARACTER_DATASET_H */
