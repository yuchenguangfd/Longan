/*
 * weighted_bipartite_max_match_unittest.cpp
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#include "weighted_bipartite_max_match.h"
#include "common/util/random.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(WeightedBipartiteMaxMatchTest, Result1Right) {
    const int N = 5;
    int mat[N][N] = {
            {3, 4, 6, 4, 9},
            {6, 4, 5, 3, 8},
            {7, 5, 3, 4, 2},
            {6, 3, 2, 2, 5},
            {8, 4, 5, 4, 7}};
    WeightedBipartite<int> bipartite(N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bipartite.AddWeight(i, j, mat[i][j]);
        }
    }
    WeightedBipartiteMaxMatch<int> match;
    std::vector<BipartiteMatchPair> maxMatch;
    int result = match.ComputeMaxMatch(bipartite, &maxMatch);
    ASSERT_EQ(29, result);
}

TEST(WeightedBipartiteMaxMatchTest, Result2Right) {
    const int N = 3;
    double mat[N][N] = {
            {3.0, 3.0, 3.0},
            {3.0, 3.0, 3.0},
            {3.0, 3.0, 3.0}};
    WeightedBipartite<double> bipartite(N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bipartite.AddWeight(i, j, mat[i][j]);
        }
    }
    WeightedBipartiteMaxMatch<double> match;
    std::vector<BipartiteMatchPair> maxMatch;
    int result = match.ComputeMaxMatch(bipartite, &maxMatch);
    ASSERT_EQ(9, result);
}

TEST(WeightedBipartiteMaxMatchTest, Result3Right) {
    const int N = 3;
    int mat[N][N] = {
            {3, 4, 60},
            {60, 4, 5},
            {7, 5, 30}};
    WeightedBipartite<int> bipartite(N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bipartite.AddWeight(i, j, mat[i][j]);
        }
    }
    WeightedBipartiteMaxMatch<int> match;
    std::vector<BipartiteMatchPair> maxMatch;
    int result = match.ComputeMaxMatch(bipartite, &maxMatch);
    ASSERT_EQ(125, result);
    ASSERT_EQ(3, maxMatch.size());
    ASSERT_TRUE(maxMatch[0].vertexLeft == 0 && maxMatch[0].vertexRight == 2);
    ASSERT_TRUE(maxMatch[1].vertexLeft == 1 && maxMatch[1].vertexRight == 0);
    ASSERT_TRUE(maxMatch[2].vertexLeft == 2 && maxMatch[2].vertexRight == 1);
}

TEST(WeightedBipartiteMaxMatchTest, BigScale) {
    const int N = 500;
    WeightedBipartite<int> bipartite(N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bipartite.AddWeight(i, j, Random::Instance().Uniform(0, 50));
        }
    }
    WeightedBipartiteMaxMatch<int> match;
    std::vector<BipartiteMatchPair> maxMatch;
    int result = match.ComputeMaxMatch(bipartite, &maxMatch);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
