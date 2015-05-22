/*
 * dijkstra_unittest.cpp
 * Created on: May 24, 2015
 * Author: chenguangyu
 */

#include "dijkstra.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(DijkstraTest, ShortestDistanceOK) {
    const int N = 6;
    WeightedDirectedGraphAsAdjMatrix<int> graph(N, 1000);
    graph.AddEdge(1, 2, 20);
    graph.AddEdge(2, 3, 30);
    graph.AddEdge(3, 4, 20);
    graph.AddEdge(4, 5, 20);
    graph.AddEdge(1, 5, 100);
    Dijkstra<int> SSSP(&graph, 1);
    SSSP.Compute();
    ASSERT_EQ(90, SSSP.Distance(5));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
