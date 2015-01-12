/*
 * algorithm_unittest.cpp
 * Created on: Nov 16, 2014
 * Author: chenguangyu
 */

#include "algorithm.h"
#include <gtest/gtest.h>
#include <utility>

using namespace longan;

TEST(AlgorithmTest, BSearchOK) {
    const int size = 1000;
    int array[size] = {};
    for (int i = 0; i < size; ++i) {
        array[i] = i*2;
    }
    for (int i = 0; i < size; ++i) {
        int pos1 = BSearch(i*2, array, size);
        ASSERT_EQ(i, pos1);
        int pos2 = BSearch(i*2+1, array, size);
        ASSERT_EQ(-1, pos2);
    }
    std::pair<int, int> array2[size];
    for (int i = 0; i < size; ++i) {
        array2[i].first = i*3;
        array2[i].second = i*3+1;
    }
    for (int i = 0; i< size; ++i) {
        int pos = BSearch(i*3, array2, size,
            [](const int& lhs, const std::pair<int, int>& rhs)->int{
                return lhs - rhs.first;
        });
        ASSERT_EQ(i, pos);
    }
}

TEST(AlgorithmTest, NextPermutationOK) {
    char str[6] = "12345";
    for (int i = 1; i < 120; ++i) {
        std::string s1(str);
        NextPermutation(str, 5);
        std::string s2(str);
        EXPECT_TRUE(s1 < s2);
    }
    EXPECT_FALSE(NextPermutation(str, 5));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
