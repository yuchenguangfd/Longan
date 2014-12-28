/*
 * polygon_unittest.cpp
 * Created on: Dec 28, 2014
 * Author: chenguangyu
 */

#include "polygon.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(PolygonTest, ComputeAreaOK) {
    Polygon<double> pg1;
    pg1.AddVertex(Point2D64F(123.45, 67.890));
    EXPECT_DOUBLE_EQ(0.0, pg1.ComputeArea());

    Polygon<int> pg2;
    pg2.AddVertex(Point2D32I(0, 0));
    pg2.AddVertex(Point2D32I(2, 0));
    pg2.AddVertex(Point2D32I(0, 2));
    EXPECT_EQ(2, pg2.ComputeArea());

    Polygon<double> pg3;
    pg3.AddVertex(Point2D64F(10, 10));
    pg3.AddVertex(Point2D64F(10, 12));
    pg3.AddVertex(Point2D64F(11, 11));
    pg3.AddVertex(Point2D64F(12, 12));
    pg3.AddVertex(Point2D64F(12, 10));
    EXPECT_DOUBLE_EQ(3.0, pg3.ComputeArea());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


