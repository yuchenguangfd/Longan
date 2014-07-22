/*
 * math_unittest.cpp
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 */

#include "math.h"
#include <gtest/gtest.h>
#include <string>

using namespace longan;
using namespace Math;

TEST(MathTest, Abs) {
    EXPECT_EQ(Abs(32), Abs(-32));
    EXPECT_EQ(Abs(3.2), Abs(-3.2));
}

TEST(MathTest, MaxMin) {
	int a = -1, b = 2;
	EXPECT_EQ(b, Max(a, b));
	EXPECT_EQ(a, Min(a, b));
	double x = 10.2, y = 4.8;
	EXPECT_EQ(x, Max(x, y));
	EXPECT_EQ(y, Min(x, y));
	std::string p = "hello", q = "world";
	EXPECT_EQ(q, Max(p, q));
	EXPECT_EQ(p, Min(p, q));
}

TEST(MathTest, RelativeError) {
    EXPECT_DOUBLE_EQ(0.0, RelativeError(100.2, 100.2));
    EXPECT_DOUBLE_EQ((101.2 - 99.0) / 99.0, RelativeError(99.0, 101.2));
    EXPECT_DOUBLE_EQ((101.2 - 99.0) / 101.2, RelativeError(101.2, 99.0));
}

TEST(MathTest, Sqr) {
	EXPECT_EQ(16, Sqr(-4));
	EXPECT_EQ(0.25, Sqr(0.5));
}

TEST(MathTest, Factorial) {
  EXPECT_EQ(1, Factorial(0));
  EXPECT_EQ(1, Factorial(1));
  EXPECT_EQ(6, Factorial(3));
  EXPECT_EQ(40320, Factorial(8));
  EXPECT_EQ(39916800, Factorial(11));
  // overflow! out of 32bit integer's range
  EXPECT_EQ(-288522240, Factorial(17));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
