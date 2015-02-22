/*
 * evaluate_rating_delegate_unittest.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "evaluate_rating_delegate.h"
#include "recsys/base/basic_predict.h"
#include "recsys/util/recsys_test_helper.h"
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
        return ItemIdList();
    }
};

TEST(EvaluateRatingDelegateTest, EvaluateRatingSTOK) {
    RatingList rlist(10, 10, 10);
    for (int i = 0; i < 10; ++i) {
        rlist.Add(RatingRecord(i, i, (i%2==0)?(float)i:(float)(-i)));
    }
    BasicPredictStub predict;
    EvaluateRatingDelegateST evaluate;
    Json::Value config;
    EvaluateOption option(config);
    evaluate.Evaluate(&predict, &rlist, &option);
    ASSERT_DOUBLE_EQ(4.5, evaluate.MAE());
    ASSERT_DOUBLE_EQ(5.338539126015656, evaluate.RMSE());
}

TEST(EvaluateRatingDelegateTest, EvaluateRatingSTAndMTResultSame) {
    RatingList rlist = RecsysTestHelper::CreateRandomRatingList(50000, 600, 1000000);
    BasicPredictStub predict;
    EvaluateRatingDelegateST evaluate1;
    EvaluateRatingDelegateMT evaluate2;
    Json::Value config;
    config["accelerate"] = true;
    EvaluateOption option(config);
    evaluate1.Evaluate(&predict, &rlist, &option);
    evaluate2.Evaluate(&predict, &rlist, &option);
    ASSERT_DOUBLE_EQ(evaluate1.MAE(), evaluate2.MAE());
    ASSERT_DOUBLE_EQ(evaluate1.RMSE(), evaluate2.RMSE());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
