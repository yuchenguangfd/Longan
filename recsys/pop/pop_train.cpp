/*
 * pop_train.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_train.h"
#include "common/logging/logging_helper.h"
#include "common/error.h"
#include <cstdio>
#include <cassert>

namespace longan {

PopTrain::PopTrain(const std::string& ratingTrainFilePath, const std::string& modelFilePath) :
    mRatingTrainFilePath(ratingTrainFilePath),
    mModelFilePath(modelFilePath),
    mNumUser(0),
    mNumItem(0),
    mNumRating(0) { }

void PopTrain::Train() {
    LOG_FUNC;
    int rtn;
    FILE* fp = fopen(mRatingTrainFilePath.c_str(), "r");
    assert(fp != nullptr);
    rtn = fscanf(fp, "%d,%d,%d", &mNumRating, &mNumUser, &mNumItem);
    assert(rtn == 3);
    InitRunningAverage();
    for (int i = 0; i < mNumRating; ++i) {
        int userId, itemId, time;
        float rating;
        rtn = fscanf(fp, "%d,%d,%f,%d", &userId, &itemId, &rating, &time);
        assert(rtn == 4);
        RatingRecord rr(userId, itemId, rating);
        AddToRunningAverage(rr);
    }
    WriteModelFile();
    DestroyRunningAverage();
}

void PopTrain::InitRunningAverage() {
    LOG_FUNC;
    mItemsAverage.resize(mNumItem);
    for (int i = 0; i < mNumItem; ++i) {
        mItemsAverage[i] = new RunningRatingAverage(0.0);
    }
    mUsersAverage.resize(mNumUser);
    for (int i = 0; i < mNumUser; ++i) {
        mUsersAverage[i] = new RunningRatingAverage(0.0);
    }
}

void PopTrain::AddToRunningAverage(const RatingRecord& rating) {
    assert(rating.ItemId() >= 0 && rating.ItemId() < mNumItem);
    assert(rating.UserId() >= 0 && rating.UserId() < mNumUser);
    mItemsAverage[rating.ItemId()]->Add(rating.Rating());
    mUsersAverage[rating.UserId()]->Add(rating.Rating());
}

/**
 * Model file(a binary file) format layout:
 * #Items(int, 4bytes)|ItemAvg_0|ItemAvg_1|...|ItemAvg_n(double, 8bytes each)
 * #Users(int, 4bytes)|UserAvg_0|UserAvg_1|...|UserAvg_m(double, 8bytes each)
 */
void PopTrain::WriteModelFile() {
    LOG_FUNC;
    FILE *fp = fopen(mModelFilePath.c_str(), "wb");
    assert(fp != nullptr);
    int rtn;
    rtn = fwrite((void*)&mNumItem, sizeof(mNumItem), 1, fp);
    assert(rtn == 1);
    for (int i = 0; i < mNumItem; ++i) {
        float avg = mItemsAverage[i]->CurrentAverage();
        rtn = fwrite((void*)&avg, sizeof(avg), 1, fp);
        assert(rtn == 1);
    }
    rtn = fwrite((void*)&mNumUser, sizeof(mNumUser), 1, fp);
    assert(rtn == 1);
    for (int i = 0; i < mNumUser; ++i) {
        float avg = mUsersAverage[i]->CurrentAverage();
        rtn = fwrite((void*)&avg, sizeof(avg), 1, fp);
        assert(rtn == 1);
    }
    fclose(fp);
}

void PopTrain::DestroyRunningAverage() {
    LOG_FUNC;
    for (int i = 0; i < mNumItem; ++i) {
        delete mItemsAverage[i];
    }
    for (int i = 0; i < mNumUser; ++i) {
        delete mUsersAverage[i];
    }
}

} //~ namespace longan
