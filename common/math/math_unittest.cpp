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

TEST(MathTest, Sqr) {
	EXPECT_EQ(16, Sqr(-4));
	EXPECT_EQ(0.25, Sqr(0.5));
}

TEST(MathTest, Factorial) {
  EXPECT_EQ(1, Math::Factorial(0));
  EXPECT_EQ(1, Math::Factorial(1));
  EXPECT_EQ(6, Math::Factorial(3));
  EXPECT_EQ(40320, Math::Factorial(8));
  EXPECT_EQ(39916800, Math::Factorial(11));
  // overflow! out of 32bit integer's range
  EXPECT_EQ(-288522240, Math::Factorial(17));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
