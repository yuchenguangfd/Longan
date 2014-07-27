/*
 * demo_gtest.cpp
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 * Show basic usage of gtest library
 */

#include <gtest/gtest.h>

int GCD(int a, int b) {
    if (a == 0 || b == 0) {
        throw "don't do that";
    }
    int c = a % b;
    if (c == 0) return b;
    return GCD(b, c);
}

TEST(GCDTest, HandleNoneZeroInput) {
    EXPECT_EQ(2, GCD(4, 10));
    EXPECT_EQ(6, GCD(30, 18));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
