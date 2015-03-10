/*
 * evaluate_coverage_delegate_unittest.cpp
 * Created on: Feb 19, 2015
 * Author: chenguangyu
 */

#include "evaluate_coverage_delegate.h"
#include "recsys/base/basic_predict.h"
#include "recsys/util/recsys_util.h"
#include <gtest/gtest.h>

using namespace longan;

class BasicPredictStub : public BasicPredict {
public:
    BasicPredictStub() : BasicPredict("", "", "") { }
    virtual void Init() override { }
    virtual void Cleanup() override { }
    virtual float PredictRating(int userId, int itemId) const { return 0.0f; }
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const {
        ItemIdList list(listSize);
        for (int i = 0; i < listSize; ++i) {
            list[i] = i;
        }
        return list;
    }
};

TEST(EvaluateCoverageDelegateTest, EvaluateCoverageSTOK) {
    RatingList testData(2, 20);
    BasicPredictStub predict;
    EvaluateCoverageDelegateST evaluate;
    Json::Value config;
    EvaluateOption option(config);
    option.SetCurrentRankingListSize(10);
    evaluate.Evaluate(&predict, &testData, &option);
    ASSERT_DOUBLE_EQ(0.5, evaluate.Coverage());
    ASSERT_DOUBLE_EQ(2.3025850929940455, evaluate.Entropy());
    ASSERT_DOUBLE_EQ(0.5263157894736842, evaluate.GiniIndex());
}

TEST(EvaluateCoverageDelegateTest, EvaluateCoverageSTAndMTResultSame) {
    RatingList testData = RecsysUtil::RandomRatingList(50000, 600, 1000000);
    BasicPredictStub predict;
    EvaluateCoverageDelegateST evaluate1;
    EvaluateCoverageDelegateMT evaluate2;
    Json::Value config;
    config["accelerate"] = true;
    EvaluateOption option(config);
    for (int size = 10; size <= 50; size += 10) {
        option.SetCurrentRankingListSize(size);
        evaluate1.Evaluate(&predict, &testData, &option);
        evaluate2.Evaluate(&predict, &testData, &option);
        ASSERT_DOUBLE_EQ(evaluate1.Coverage(), evaluate2.Coverage());
        ASSERT_DOUBLE_EQ(evaluate1.Entropy(), evaluate2.Entropy());
        ASSERT_DOUBLE_EQ(evaluate1.GiniIndex(), evaluate2.GiniIndex());
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
