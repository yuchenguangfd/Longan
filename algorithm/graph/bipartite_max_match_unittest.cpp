/*
 * bipartite_max_match_unittest.cpp
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#include "bipartite_max_match.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(BipartiteMaxMatchTest, ResultRight) {
    const int N = 3;
    int mat[N][N] = {
            {1, 0, 1},
            {0, 1, 0},
            {0, 1, 0}
    };
    Bipartite<> bipartite(N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (mat[i][j] > 0) bipartite.AddEdge(i, j);
        }
    }
    BipartiteMaxMatch<> match;
    int result = match.ComputeMaxMatch(bipartite);
    ASSERT_EQ(2, result);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
