/*
 * array_unittest.cpp
 * Created on: May 12, 2015
 * Author: chenguangyu
 */

#include "array.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(Array1DTest, AllOk) {
    IntArray arr1(5);
    DoubleArray arr2(5);
    Array1D<std::string> arr3(5, true);
    Array1D<std::string> arr4(5, true, "hello");
    for (int i = 0; i < arr2.Size(); ++i) {
        arr2[i] = i * 0.1;
    }
    for (int i = 0; i < arr4.Size(); ++i) {
        EXPECT_EQ("hello", arr4[i]);
    }
}


TEST(Array2DTest, AllOK) {
    Array2D<std::string> array(3, 3, true, "hello");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ASSERT_EQ(array[i][j], "hello");
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}




