/*
 * bellmanford_unittest.cpp
 * Created on: May 28, 2015
 * Author: chenguangyu
 */

#include "bellmanford.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(BellmanFordTest, FindNegativeCycleOK) {
    WeightedDirectedGraphAsEdgeList<int> graph1(3, 7);
    graph1.AddEdge(0, 1, 2);graph1.AddEdge(1, 0, 2);
    graph1.AddEdge(0, 2, 4);graph1.AddEdge(2, 0, 4);
    graph1.AddEdge(1, 2, 1);graph1.AddEdge(2, 1, 1);
    graph1.AddEdge(2, 0, -3);
    BellmanFord<int, int> SSSP1(&graph1, 0);
    SSSP1.Compute();
    ASSERT_FALSE(SSSP1.ExistNegativeCycle());

    WeightedDirectedGraphAsEdgeList<int> graph2(3, 7);
    graph2.AddEdge(0, 1, 3);graph2.AddEdge(1, 0, 3);
    graph2.AddEdge(1, 2, 4);graph2.AddEdge(2, 1, 4);
    graph2.AddEdge(2, 0, -8);
    BellmanFord<int, int> SSSP2(&graph2, 0);
    SSSP2.Compute();
    ASSERT_TRUE(SSSP2.ExistNegativeCycle());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
