/*
 * basic_evaluate_unittest.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_evaluate.h"
#include <gtest/gtest.h>

using namespace longan;

class BasicPredictStub : public BasicPredict {
public:
    BasicPredictStub() : BasicPredict("", "", "") { }
    virtual void Init() override { }
    virtual void Cleanup() override { }
    virtual float PredictRating(int userId, int itemId) const {
        return 0.0f;
    }
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const {
        ItemIdList list(listSize);
        for (int i = 0; i < listSize; ++i) {
            list[i] = i;
        }
        return list;
    }
};

class BasicEvaluateTest : public ::testing::Test, public longan::BasicEvaluate {
public:
    BasicEvaluateTest() : BasicEvaluate("","","","","") { }
protected:
    virtual void CreatePredict() {
        mPredict = new BasicPredictStub();
    }
};

TEST_F(BasicEvaluateTest, EvaluateRatingOK) {
    for (int i = 0; i < 10; ++i) {
        mTestRatingList.Add(RatingRecord(i, i, (i%2==0)?(float)i:(float)(-i)));
    }
    CreatePredict();
    EvaluateRating();
    DestroyPredict();
    ASSERT_DOUBLE_EQ(4.5, mMAE);
    ASSERT_DOUBLE_EQ(5.338539126015656, mRMSE);
}

TEST_F(BasicEvaluateTest, EvaluateRankingOK) {
    mConfig["rankingListSize"] = 10;
    RatingList rlist(2, 40);
    for (int i = 0; i < 30; i += 2) {
        rlist.Add(RatingRecord(0, i, 1.0f));
    }
    for (int i = 0; i < 40; i += 3) {
        rlist.Add(RatingRecord(1, i, 1.0f));
    }
    mTestRatingList = std::move(rlist);
    CreatePredict();
    EvaluateRanking();
    DestroyPredict();
    ASSERT_DOUBLE_EQ(0.45, mPrecision);
    ASSERT_DOUBLE_EQ(0.30952380952380953, mRecall);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
