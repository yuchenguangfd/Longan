/*
 * next_combination_unittest.cpp
 * Created on: May 5, 2015
 * Author: chenguangyu
 */

#include <gtest/gtest.h>
#include "next.h"

using namespace longan;

TEST(NextCombinationTest, TotalNumOK) {
    int n = 5, r = 3;
    int array[] = {0, 1, 2};
    int count = 1;
    while (NextCombination(array, n, r)) ++count;
    ASSERT_EQ(10, count);
}

TEST(NextPermutationTest, TotalNumOK) {
    int n = 5;
    int array[] = {0, 1, 2, 3, 4};
    int count = 1;
    while (NextPermutation(array, n)) ++count;
    ASSERT_EQ(120, count);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
