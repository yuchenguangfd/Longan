/*
 * characterdataset.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: chenguangyu
 */

#include "character_dataset.h"

namespace longan {

CharacterDataset::CharacterDataset(const std::string& ratingFilepath, const std::string& resultFilepath) :
    mRatingFilepath(ratingFilepath),
    mResultFilepath(resultFilepath) { }

void CharacterDataset::Character() {
    mRatingData = RatingList::LoadFromTextFile(mRatingFilepath);
    mRatingTrait.Init(mRatingData);
    ReportMeta();
}

void CharacterDataset::ReportMeta() {
    Json::Value & meta = mResult["meta"];
    meta["numUser"] = mRatingData.NumUser();
}

} //~ namespace longan
