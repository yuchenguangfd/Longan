/*
 * polygon_unittest.cpp
 * Created on: Dec 28, 2014
 * Author: chenguangyu
 */

#include "rect_union_area.h"
#include <gtest/gtest.h>

using namespace longan;


TEST(RectUnionAreaTest, ComputeAreaOK) {
    RectUnionArea<double> unionArea;
    std::vector<Rect64F> rects1{
        Rect64F(10, 10, 10, 10),
        Rect64F(15, 15, 10, 10.5)
    };
    EXPECT_DOUBLE_EQ(180.0, unionArea.ComputeUnionArea(rects1));
    std::vector<Rect64F> rects2{
        Rect64F(1.37, 0.10, 1.38, 0.10),
        Rect64F(2.96, 2.01, 0.10, 0.74),
        Rect64F(2.65, 1.05, 2.32, 0.10),
        Rect64F(2.33, 0.74, 1.69, 0.10),
        Rect64F(2.96, 0.10, 1.06, 1.37),
    };
    EXPECT_DOUBLE_EQ(1.8532, unionArea.ComputeUnionArea(rects2));
    std::vector<Rect64F> rects3{
            Rect64F(9.33, 19.52, 21.11, 4.87),
            Rect64F(8.06, 29.38, 1.69, 22.39),
            Rect64F(22.38, 15.06, 9.97, 18.24),
            Rect64F(30.66, 19.20, 13.47, 0.42),
            Rect64F(17.93, 15.38, 21.42, 26.20),
            Rect64F(9.65,  6.47, 20.15, 3.92),
            Rect64F(26.20, 24.61, 14.43, 11.24),
            Rect64F(25.25, 12.51, 4.55, 23.66)
    };
    EXPECT_DOUBLE_EQ(754.7506, unionArea.ComputeUnionArea(rects3));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
