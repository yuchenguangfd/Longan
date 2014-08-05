/*
 * integer_unittest.cpp
 * Created on: Aug 4, 2014
 * Author: chenguangyu
 */

#include "integer.h"
#include <gtest/gtest.h>
#include <string>

using namespace longan;
using namespace std;

TEST(IntegerTest, ParseInt) {
    string s1 = "-12034";
    string s2 = "01234";
    string s3 = "  +012hello ";
    EXPECT_EQ(-12034, Integer::ParseInt(s1));
    EXPECT_EQ(1234, Integer::ParseInt(s2));
    EXPECT_EQ(12, Integer::ParseInt(s3));
}

TEST(IntegerTest, ToString) {
    int i1 = 1234;
    int i2 = -4321;
    int i3 = +2147483647;
    EXPECT_EQ("1234", Integer::ToString(i1));
    EXPECT_EQ("-4321", Integer::ToString(i2));
    EXPECT_EQ("2147483647", Integer::ToString(i3));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
