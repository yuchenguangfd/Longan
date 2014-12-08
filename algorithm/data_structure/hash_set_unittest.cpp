/*
 * hash_set.h
 * Created on: Dec 7, 2014
 * Author: chenguangyu
 */

#include "hash_set.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(HashSetChainedTest, InsertAndFindOK) {
    struct Hash {
        int operator() (int key) { return ((key >> 16)^key) & 0xFFFF; }
    };
    struct Pred {
        bool operator() (int lhs, int rhs) { return lhs == rhs; }
    };
    HashSetChained<int, Hash, Pred> hashsetImp(65536);
    HashSet<int>& hashset = hashsetImp;
    int N = 1000000;
    for (int i = 0; i < N; i += 2) {
        EXPECT_TRUE(hashset.Insert(i));
    }
    for (int i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(i, *(hashset.Find(i)));
        } else {
            EXPECT_EQ(nullptr, hashset.Find(i));
        }
    }
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
