/*
 * cf_auto_encoder_computation_unittest.cpp
 * Created on: Mar 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_model_computation.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace longan::CFAE;

TEST(ModelComputationTest, HammingDistanceOK) {
    float data1[] = {0.1, 0.2, 0.3, 0.4, 0.55, 0.6, 0.7, 0.8, 0.9};
    float data2[] = {0.1, 0.3, 0.55, 0.7, 0.9, 0.2, 0.4, 0.6, 0.8};
    BinaryCode code1(Vector64F(data1, data1 + 9));
    BinaryCode code2(Vector64F(data2, data2 + 9));
    ASSERT_EQ(4, HammingDistance(code1, code2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
