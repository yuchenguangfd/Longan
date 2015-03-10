/*
 * item_based_model_computation_unittest.cpp
 * Created on: Nov 9, 2014
 *  Author: chenguangyu
 */

#include "item_based_model_computation.h"
#include "recsys/util/recsys_util.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace longan::ItemBased;

TEST(ModelComputationTest, ComputeSimilarityOK) {
    RatingRecord data1[] = {RatingRecord(2,1,4.0), RatingRecord(3,1,2.0), RatingRecord(4,1,6.0), RatingRecord(5,1,9.0)};
    RatingRecord data2[] = {RatingRecord(1,2,3.0), RatingRecord(3,2,5.0), RatingRecord(4,2,7.0)};
    ItemVec iv1(1, data1, sizeof(data1) / sizeof(data1[0]));
    ItemVec iv2(2, data2, sizeof(data2) / sizeof(data2[0]));
    class ModelComputationMock : public ModelComputation {
    public:
        virtual void ComputeModel(const TrainOption *option, RatingMatItems *trainData,
                Model *model) { }
        float ComputeCosineSimilarityMock(ItemVec& iv1, ItemVec& iv2) {
            return ComputeCosineSimilarity(iv1, iv2);
        }
        float ComputeBinaryCosineSimilarityMock(ItemVec& iv1, ItemVec& iv2) {
            return ComputeBinaryCosineSimilarity(iv1, iv2);
        }
        float ComputeBinaryJaccardSimilarityMock(ItemVec& iv1, ItemVec& iv2) {
            return ComputeBinaryJaccardSimilarity(iv1, iv2);
        }
    };
    ASSERT_FLOAT_EQ(0.9557790f, ModelComputationMock().ComputeCosineSimilarityMock(iv1, iv2));
    ASSERT_FLOAT_EQ(0.5773502f, ModelComputationMock().ComputeBinaryCosineSimilarityMock(iv1, iv2));
    ASSERT_FLOAT_EQ(0.4f, ModelComputationMock().ComputeBinaryJaccardSimilarityMock(iv1, iv2));
}

TEST(ModelComputationTest, ComputeModelSTAndMTResultSame) {
    Json::Value config;
    config["parameter"]["ratingType"] = "numerical";
    config["parameter"]["simType"] = "cosine";
    config["trainOption"]["accelerate"] = true;
    TrainOption option(config["trainOption"]);
    Parameter parameter(config["parameter"]);
    int numUser = 60;
    int numItem = 5000;
    int numRating = 30000;
    RatingMatItems trainData = RecsysUtil::RandomRatingMatItems(numUser, numItem, numRating);
    Model model1(&parameter, numItem);
    Model model2(&parameter, numItem);
    ModelComputationST comp1;
    ModelComputationMT comp2;
    comp1.ComputeModel(&option, &trainData, &model1);
    comp2.ComputeModel(&option, &trainData, &model2);
    for (int iid1 = 0; iid1 < numItem; ++iid1) {
        for (int iid2 = iid1 + 1; iid2 < numItem; ++iid2) {
            ASSERT_FLOAT_EQ(model1.Get(iid1, iid2), model2.Get(iid1, iid2));
        }
    }
}

TEST(ModelComputationTest, ComputeModelMTAndMTStaticScheduleResultSame) {
    Json::Value config;
    config["parameter"]["ratingType"] = "binary";
    config["parameter"]["simType"] = "binaryCosine";
    config["trainOption"]["accelerate"] = true;
    TrainOption option(config["trainOption"]);
    Parameter parameter(config["parameter"]);
    int numUser = 60;
    int numItem = 5000;
    int numRating = 30000;
    RatingMatItems trainData = RecsysUtil::RandomRatingMatItems(numUser, numItem, numRating);
    Model model1(&parameter, numItem);
    Model model2(&parameter, numItem);
    ModelComputationMT comp1;
    ModelComputationMTStaticSchedule comp2;
    comp1.ComputeModel(&option, &trainData, &model1);
    comp2.ComputeModel(&option, &trainData, &model2);
    for (int iid1 = 0; iid1 < numItem; ++iid1) {
        for (int iid2 = iid1 + 1; iid2 < numItem; ++iid2) {
            ASSERT_FLOAT_EQ(model1.Get(iid1, iid2), model2.Get(iid1, iid2));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
