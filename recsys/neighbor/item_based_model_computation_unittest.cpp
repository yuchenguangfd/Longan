/*
 * item_based_model_computation_unittest.cpp
 * Created on: Nov 9, 2014
 *  Author: chenguangyu
 */

#include "item_based_model_computation.h"
#include "recsys/util/recsys_test_helper.h"
#include "common/time/stopwatch.h"
#include "common/system/system_info.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace longan::ItemBased;

class ModelComputationTest : public ::testing::Test, public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsItems<>*, ModelTrain*) { }
};

TEST_F(ModelComputationTest, ComputeSimilarityOK) {
    RatingRecord data1[] = {RatingRecord(2,1,4.0), RatingRecord(3,1,2.0), RatingRecord(4,1,6.0), RatingRecord(5,1,9.0)};
    RatingRecord data2[] = {RatingRecord(1,2,3.0), RatingRecord(3,2,5.0), RatingRecord(4,2,7.0)};
    ItemVector<> iv1(1, data1, sizeof(data1) / sizeof(data1[0]));
    ItemVector<> iv2(2, data2, sizeof(data2) / sizeof(data2[0]));
    ASSERT_FLOAT_EQ(0.9557790f ,ComputeSimilarity(iv1, iv2));
}

TEST(DynamicScheduledModelComputationTest, DynamicScheduledAndSimpleResultSame) {
    int numUser = 60;
    int numItem = 5000;
    int numRating = 30000;
    int neighborSize = 50;
    RatingMatrixAsItems<> rmat = RecsysTestHelper::CreateRandomRatingMatrixAsItems(numUser, numItem, numRating);
    FixedNeighborSizeModel model1(numItem, neighborSize);
    FixedNeighborSizeModel model2(numItem, neighborSize);
    SimpleModelComputation comp1;
    Stopwatch sw;
    sw.Start();
    comp1.ComputeModel(&rmat, &model1);
    printf("Simple ModelComputation time cost = %.0lfms\n", sw.ElapsedMilliseconds());
    DynamicScheduledModelComputation comp2(SystemInfo::GetNumCPUCore());
    sw.Reset(); sw.Start();
    comp2.ComputeModel(&rmat, &model2);
    printf("DynamicScheduled ModelComputation time cost =%.0lfms\n", sw.ElapsedMilliseconds());
    for (int iid = 0; iid < numItem; ++iid) {
        const NeighborItem* begin1 = model1.NeighborBegin(iid);
        const NeighborItem* end1 = model1.NeighborEnd(iid);
        const NeighborItem* begin2 = model2.NeighborBegin(iid);
        const NeighborItem* end2 = model2.NeighborEnd(iid);
        std::vector<NeighborItem> neighbor1(begin1, end1);
        std::vector<NeighborItem> neighbor2(begin2, end2);
        ASSERT_EQ(neighbor1.size(), neighbor2.size());
        std::sort(neighbor1.begin(), neighbor1.end());
        std::sort(neighbor2.begin(), neighbor2.end());
        for (int i = 0; i < neighbor1.size(); ++i) {
            ASSERT_FLOAT_EQ(neighbor1[i].Similarity(), neighbor2[i].Similarity());
        }
    }
}

TEST(StaticScheduledModelComputationTest, StaticScheduledAndSimpleResultSame) {
    int numUser = 60;
    int numItem = 5000;
    int numRating = 30000;
    float threshold = 0.5f;
    RatingMatrixAsItems<> rmat = RecsysTestHelper::CreateRandomRatingMatrixAsItems(numUser, numItem, numRating);
    FixedSimilarityThresholdModel model1(numItem, threshold);
    FixedSimilarityThresholdModel model2(numItem, threshold);
    StaticScheduledModelComputation comp1;
    Stopwatch sw;
    sw.Start();
    comp1.ComputeModel(&rmat, &model1);
    printf("StaticScheduled ModelComputation time cost = %.0lfms\n", sw.ElapsedMilliseconds());

    DynamicScheduledModelComputation comp2(SystemInfo::GetNumCPUCore());
    sw.Reset(); sw.Start();
    comp2.ComputeModel(&rmat, &model2);
    printf("DynamicScheduled ModelComputation time cost =%.0lfms\n", sw.ElapsedMilliseconds());
    for (int iid = 0; iid < numItem; ++iid) {
        const NeighborItem* begin1 = model1.NeighborBegin(iid);
        const NeighborItem* end1 = model1.NeighborEnd(iid);
        const NeighborItem* begin2 = model2.NeighborBegin(iid);
        const NeighborItem* end2 = model2.NeighborEnd(iid);
        std::vector<NeighborItem> neighbor1(begin1, end1);
        std::vector<NeighborItem> neighbor2(begin2, end2);
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
