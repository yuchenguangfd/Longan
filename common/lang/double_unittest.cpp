 /*
 * double_unittest.cpp
 * Created on: Sep 8, 2014
 * Author: chenguangyu
 */

#include "double.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(DoubleTest, DblBlablaOK) {
    ASSERT_EQ(0, Double::DblCmp(0.0, 0.01));
    ASSERT_EQ(0, Double::DblCmp(0.0001, 0.01));
    ASSERT_EQ(0, Double::DblCmp(-0.0001, 0.01));
    ASSERT_EQ(1, Double::DblCmp(10));
    ASSERT_EQ(-1, Double::DblCmp(-10));

    ASSERT_TRUE(Double::DblEqual(1.234, 1.234));
    ASSERT_TRUE(Double::DblEqual(1.234, 1.235, 0.01));
    ASSERT_FALSE(Double::DblEqual(1.234, 1.235, 0.00001));

    ASSERT_TRUE(Double::DblNotEqual(1.234, 12.34));
    ASSERT_TRUE(Double::DblNotEqual(1.234, 1.235, 0.00001));

    ASSERT_TRUE(Double::DblZero(0.0));
    ASSERT_TRUE(Double::DblZero(-1e-30));
    ASSERT_FALSE(Double::DblZero(-0.001, 0.000001));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

