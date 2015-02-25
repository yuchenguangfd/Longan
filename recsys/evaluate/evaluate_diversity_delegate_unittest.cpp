/*
 * evaluate_diversity_delegate_unittest.cpp
 * Created on: Feb 19, 2015
 * Author: chenguangyu
 */

#include "evaluate_diversity_delegate.h"
#include "recsys/base/basic_predict.h"
#include "recsys/util/recsys_test_helper.h"
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
    virtual float ComputeItemSimilarity(int itemId1, int itemId2) const {
        return 0.75f;
    }
};

TEST(EvaluateDiversityDelegateTest, EvaluateDiversitySTOK) {
    RatingList testData(40, 20);
    BasicPredictStub predict;
    EvaluateDiversityDelegateST evaluate;
    Json::Value config;
    EvaluateOption option(config);
    option.SetCurrentRankingListSize(10);
    evaluate.Evaluate(&predict, &testData, &option);
    ASSERT_DOUBLE_EQ(0.25, evaluate.Diversity());
}

TEST(EvaluateDiversityDelegateTest, EvaluateDiversitySTAndMTResultSame) {
    RatingList testData = RecsysTestHelper::CreateRandomRatingList(50000, 600, 1000000);
    BasicPredictStub predict;
    EvaluateDiversityDelegateST evaluate1;
    EvaluateDiversityDelegateMT evaluate2;
    Json::Value config;
    config["accelerate"] = true;
    EvaluateOption option(config);
    for (int size = 10; size <= 50; size += 10) {
        option.SetCurrentRankingListSize(size);
        evaluate1.Evaluate(&predict, &testData, &option);
        evaluate2.Evaluate(&predict, &testData, &option);
        ASSERT_DOUBLE_EQ(evaluate1.Diversity(), evaluate2.Diversity());
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
