/*
 * graph_unittest.cpp
 * Created on: Jan 12, 2015
 * Author: chenguangyu
 */

#include "graph.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(UndirectedGraphAsAdjMatrixTest, DegreeOK) {
    const int N = 4, M = 4;
    int data[N][2] = { {0, 1}, {0, 3}, {1, 2}, {2, 3}};
    UndirectedGraphAsAdjMatrix<> graph(N);
    for (int i = 0; i < M; ++i) {
        graph.AddEdge(Edge(data[i][0], data[i][1]));
    }
    EXPECT_EQ(2, graph.DegreeOfVertex(0));
    EXPECT_EQ(2, graph.DegreeOfVertex(1));
    EXPECT_EQ(2, graph.DegreeOfVertex(2));
    EXPECT_EQ(2, graph.DegreeOfVertex(3));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

