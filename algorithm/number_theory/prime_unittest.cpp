/*
 * prime_unittest.cpp
 * Created on: Jan 4, 2015
 * Author: chenguangyu
 */

#include "prime.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(PrimeTest, PrimeFactorizationOK) {
    PrimeFactorizationResult result;

    PrimeFactorization(36, &result);
    EXPECT_TRUE(2 == result.Size());
    EXPECT_TRUE(2 == result.Factor(0) && 2 == result.Power(0));
    EXPECT_TRUE(3 == result.Factor(1) && 2 == result.Power(1));

    PrimeFactorization(42, &result);
    EXPECT_TRUE(3 == result.Size());
    EXPECT_TRUE(2 == result.Factor(0) && 1 == result.Power(0));
    EXPECT_TRUE(3 == result.Factor(1) && 1 == result.Power(1));
    EXPECT_TRUE(7 == result.Factor(2) && 1 == result.Power(2));

    PrimeFactorization(2147483647, &result);
    EXPECT_TRUE(1 == result.Size());

    PrimeFactorization(3992003, &result);
    EXPECT_TRUE(2 == result.Size());
    EXPECT_TRUE(1997 == result.Factor(0) && 1 == result.Power(0));
    EXPECT_TRUE(1999 == result.Factor(1) && 1 == result.Power(1));

    PrimeFactorization(1, &result);
    EXPECT_TRUE(0 == result.Size());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
