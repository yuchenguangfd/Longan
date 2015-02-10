/*
 * evaluate_ranking_evaluate_unittest.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "evaluate_ranking_delegate.h"
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
};

TEST(EvaluateRankingDelegateTest, EvaluateRankingSTOK) {
    RatingList rlist(2, 40);
    for (int i = 0; i < 30; i += 2) {
        rlist.Add(RatingRecord(0, i, 1.0f));
    }
    for (int i = 0; i < 40; i += 3) {
        rlist.Add(RatingRecord(1, i, 1.0f));
    }
    BasicPredictStub predict;
    RatingMatUsers testData;
    testData.Init(rlist);
    EvaluateRankingDelegateST evaluate;
    Json::Value config;
    config["evaluateRanking"] = true;
    config["rankingListSize"] = 10;
    EvaluateOption option(config);
    evaluate.Evaluate(&predict, &testData, &option);
    ASSERT_DOUBLE_EQ(0.45, evaluate.Precision());
    ASSERT_DOUBLE_EQ(0.31034482758620691, evaluate.Recall());
    ASSERT_DOUBLE_EQ(0.3673469387755102, evaluate.F1Score());
}

TEST(EvaluateRankingDelegateTest, EvaluateRankingSTAndMTResultSame) {
    BasicPredictStub predict;
    RatingMatUsers testData = RecsysTestHelper::CreateRandomRatingMatrixAsUsers(500, 600, 10000);
    Json::Value config;
    config["evaluateRanking"] = true;
    config["rankingListSize"] = 50;
    config["numThread"] = 4;
    EvaluateOption option(config);
    EvaluateRankingDelegateST evaluate1;
    EvaluateRankingDelegateMT evaluate2;
    evaluate1.Evaluate(&predict, &testData, &option);
    evaluate2.Evaluate(&predict, &testData, &option);
    ASSERT_DOUBLE_EQ(evaluate1.Precision(), evaluate2.Precision());
    ASSERT_DOUBLE_EQ(evaluate1.Recall(), evaluate2.Recall());
    ASSERT_DOUBLE_EQ(evaluate1.F1Score(), evaluate2.F1Score());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
