/*
 * math_unittest.cpp
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 */

#include "math.h"
#include "common/lang/defines.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

using namespace longan;
using namespace Math;

TEST(MathTest, AbsOK) {
    EXPECT_EQ(Abs(32), Abs(-32));
    EXPECT_EQ(Abs(3.2), Abs(-3.2));
}

TEST(MathTest, MaxMinOK) {
	int a = -1, b = 2, c = 3;
	EXPECT_EQ(b, Max(a, b));
	EXPECT_EQ(a, Min(a, b));
	EXPECT_EQ(c, Max(a, b, c));
	EXPECT_EQ(a, Min(a, b, c));
	double x = 10.2, y = 4.8, z = -3.2;
	EXPECT_EQ(x, Max(x, y));
	EXPECT_EQ(y, Min(x, y));
	EXPECT_EQ(x, Max(x, y, z));
	EXPECT_EQ(z, Min(x, y, z));
	std::string p = "hello", q = "world", w = "000";
	EXPECT_EQ(q, Max(p, q));
	EXPECT_EQ(p, Min(p, q));
	EXPECT_EQ(q, Max(p, q, w));
	EXPECT_EQ(w, Min(p, q, w));
}

TEST(MathTest, MaxMinInRangeOK) {
    int array[] = {123, 456, 789, -321, -67, 0};
    int *p1;
    ASSERT_EQ(789, MaxInRange(array, array + arraysize(array)));
    ASSERT_EQ(789, MaxInRange(array, array + arraysize(array), p1));
    ASSERT_EQ(&array[2], p1);
    ASSERT_EQ(-321, MinInRange(array, array + arraysize(array)));
    ASSERT_EQ(-321, MinInRange(array, array + arraysize(array), p1));
    ASSERT_EQ(&array[3], p1);
    std::vector<double> vec = {1.23, 4.56, 7.89, -3.21, -0.67, 0};
    std::vector<double>::iterator p2;
    ASSERT_EQ(7.89, MaxInRange(vec.begin(), vec.end()));
    ASSERT_EQ(7.89, MaxInRange(vec.begin(), vec.end(), p2));
    ASSERT_EQ(vec.begin() + 2, p2);
    ASSERT_EQ(-3.21, MinInRange(vec.begin(), vec.end()));
    ASSERT_EQ(-3.21, MinInRange(vec.begin(), vec.end(), p2));
    ASSERT_EQ(vec.begin() + 3, p2);
}

TEST(MathTest, RelativeErrorOK) {
    EXPECT_DOUBLE_EQ(0.0, RelativeError(100.2, 100.2));
    EXPECT_DOUBLE_EQ((101.2 - 99.0) / 99.0, RelativeError(99.0, 101.2));
    EXPECT_DOUBLE_EQ((101.2 - 99.0) / 101.2, RelativeError(101.2, 99.0));
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
