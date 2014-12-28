/*
 * user_based_model_computation_unittest.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_model_computation.h"
#include "recsys/util/recsys_test_helper.h"
#include "common/time/stopwatch.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace longan::UserBased;

class ModelComputationTest : public ::testing::Test, public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsUsers<>*, ModelTrain*) { }
};

TEST_F(ModelComputationTest, ComputeSimilarityOK) {
    RatingRecord data1[] = {RatingRecord(1,2,4.0), RatingRecord(1,3,2.0), RatingRecord(1,4,6.0), RatingRecord(1,5,9.0)};
    RatingRecord data2[] = {RatingRecord(2,1,3.0), RatingRecord(2,3,5.0), RatingRecord(2,4,7.0)};
    UserVector<> uv1(1, data1, sizeof(data1) / sizeof(data1[0]));
    UserVector<> uv2(2, data2, sizeof(data2) / sizeof(data2[0]));
    ASSERT_FLOAT_EQ(0.9557790f ,ComputeSimilarity(uv1, uv2));
}

TEST(DynamicScheduledModelComputationTest, DynamicScheduledAndSimpleResultSame) {
    int numUser = 6000;
    int numItem = 50;
    int numRating = 30000;
    int neighborSize = 50;
    RatingMatrixAsUsers<> rmat = RecsysTestHelper::CreateRandomRatingMatrixAsUsers(numUser, numItem, numRating);
    FixedNeighborSizeModel model1(numUser, neighborSize);
    FixedNeighborSizeModel model2(numUser, neighborSize);
    SimpleModelComputation comp1;
    Stopwatch sw;
    sw.Start();
    comp1.ComputeModel(&rmat, &model1);
    printf("Simple ModelComputation time cost = %.0lfms\n", sw.ElapsedMilliseconds());
    DynamicScheduledModelComputation comp2;
    sw.Reset(); sw.Start();
    comp2.ComputeModel(&rmat, &model2);
    printf("DynamicScheduled ModelComputation time cost =%.0lfms\n", sw.ElapsedMilliseconds());
    for (int uid = 0; uid < numUser; ++uid) {
        const NeighborUser* begin1 = model1.NeighborBegin(uid);
        const NeighborUser* end1 = model1.NeighborEnd(uid);
        const NeighborUser* begin2 = model2.NeighborBegin(uid);
        const NeighborUser* end2 = model2.NeighborEnd(uid);
        std::vector<NeighborUser> neighbor1(begin1, end1);
        std::vector<NeighborUser> neighbor2(begin2, end2);
        ASSERT_EQ(neighbor1.size(), neighbor2.size());
        std::sort(neighbor1.begin(), neighbor1.end());
        std::sort(neighbor2.begin(), neighbor2.end());
        for (int i = 0; i < neighbor1.size(); ++i) {
            ASSERT_FLOAT_EQ(neighbor1[i].Similarity(), neighbor2[i].Similarity());
        }
    }
}

TEST(StaticScheduledModelComputationTest, StaticScheduledAndSimpleResultSame) {
    int numUser = 6000;
    int numItem = 50;
    int numRating = 30000;
    float threshold = 0.5f;
    RatingMatrixAsUsers<> rmat = RecsysTestHelper::CreateRandomRatingMatrixAsUsers(numUser, numItem, numRating);
    FixedSimilarityThresholdModel model1(numUser, threshold);
    FixedSimilarityThresholdModel model2(numUser, threshold);

    StaticScheduledModelComputation comp1;
    Stopwatch sw;
    sw.Start();
    comp1.ComputeModel(&rmat, &model1);
    printf("StaticScheduled ModelComputation time cost = %.0lfms\n", sw.ElapsedMilliseconds());

    DynamicScheduledModelComputation comp2;
    sw.Reset(); sw.Start();
    comp2.ComputeModel(&rmat, &model2);
    printf("DynamicScheduled ModelComputation time cost =%.0lfms\n", sw.ElapsedMilliseconds());
    for (int uid = 0; uid < numUser; ++uid) {
        const NeighborUser* begin1 = model1.NeighborBegin(uid);
        const NeighborUser* end1 = model1.NeighborEnd(uid);
        const NeighborUser* begin2 = model2.NeighborBegin(uid);
        const NeighborUser* end2 = model2.NeighborEnd(uid);
        std::vector<NeighborUser> neighbor1(begin1, end1);
        std::vector<NeighborUser> neighbor2(begin2, end2);
        ASSERT_EQ(neighbor1.size(), neighbor2.size());
        std::sort(neighbor1.begin(), neighbor1.end());
        std::sort(neighbor2.begin(), neighbor2.end());
        for (int i = 0; i < neighbor1.size(); ++i) {
            ASSERT_FLOAT_EQ(neighbor1[i].Similarity(), neighbor2[i].Similarity());
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
