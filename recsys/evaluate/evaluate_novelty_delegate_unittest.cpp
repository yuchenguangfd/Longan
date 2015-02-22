/*
 * evaluate_novelty_delegate_unittest.cpp
 * Created on: Feb 19, 2015
 * Author: chenguangyu
 */

#include "evaluate_novelty_delegate.h"
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

TEST(EvaluateNoveltyDelegateTest, EvaluateNoveltySTOK) {
    RatingList trainData(2, 5);
    trainData.Add(RatingRecord(0, 0, 0.0));
    trainData.Add(RatingRecord(0, 1, 1.0));
    trainData.Add(RatingRecord(1, 2, 2.0));
    trainData.Add(RatingRecord(1, 3, 3.0));
    trainData.Add(RatingRecord(1, 4, 4.0));
    BasicPredictStub predict;
    EvaluateNoveltyDelegateST evaluate;
    Json::Value config;
    config["evaluateNovelty"] = true;
    config["rankingListSize"] = 5;
    EvaluateOption option(config);
    evaluate.Evaluate(&predict, &trainData, &option);
    ASSERT_DOUBLE_EQ(0.9574983485564091, evaluate.Novelty());
}

TEST(EvaluateNoveltyDelegateTest, EvaluateNoveltySTAndMTResultSame) {
    RatingList trainData = RecsysTestHelper::CreateRandomRatingList(50000, 600, 1000000);
    BasicPredictStub predict;
    EvaluateNoveltyDelegateST evaluate1;
    EvaluateNoveltyDelegateMT evaluate2;
    Json::Value config;
    config["accelerate"] = true;
    config["evaluateNovelty"] = true;
    config["rankingListSize"] = 50;
    EvaluateOption option(config);
    evaluate1.Evaluate(&predict, &trainData, &option);
    evaluate2.Evaluate(&predict, &trainData, &option);
    ASSERT_FLOAT_EQ(evaluate1.Novelty(), evaluate2.Novelty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
