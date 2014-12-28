/*
 * point_unittest.cpp
 * Created on: Dec 28, 2014
 * Author: chenguangyu
 */

#include "point.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(Point2DTest, ConstructOK) {
    Point2D<int> p1;
    EXPECT_EQ(0, p1.x);
    EXPECT_EQ(0, p1.y);
    Point2D<int> p2(-1, 2);
    EXPECT_EQ(-1, p2.x);
    EXPECT_EQ(2, p2.y);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
