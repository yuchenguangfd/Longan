/*
 * number_theory_unittest.cpp
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#include "number_theory.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace NumberTheory;

TEST(NumberTheoryTest, Gcd) {
    EXPECT_EQ(2, Gcd(4, 10));
    EXPECT_EQ(2, Gcd(10, 4));
    EXPECT_EQ(18, Gcd(36, 18));
    EXPECT_EQ(4, Gcd(2000000000, -1999999996));
    EXPECT_EQ(4, NumberTheory::Gcd(0, 4));
    EXPECT_EQ(0, NumberTheory::Gcd(0, 0));
}

TEST(NumberTheoryTest, IsPrime) {
    int cnt = 0;
    for (int i = 0; i < 100; ++i) {
        cnt += IsPrime(i)? 1 : 0;
    }
    EXPECT_EQ(25, cnt);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
