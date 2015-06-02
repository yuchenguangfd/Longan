/*
 * prim_unittest.cpp
 * Created on: Jun 2, 2015
 * Author: chenguangyu
 */

#include "prim.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(PrimTest, MSTTotalWeightOK) {
    const int N = 4;
    WeightedDirectedGraphAsAdjMatrix<int> graph(N);
    graph.AddEdge(0, 1, 4); graph.AddEdge(0, 2, 9);graph.AddEdge(0, 3, 21);
    graph.AddEdge(1, 0, 4);graph.AddEdge(1, 2, 8);graph.AddEdge(1, 3, 17);
    graph.AddEdge(2, 0, 9);graph.AddEdge(2, 1, 8);graph.AddEdge(2, 3, 16);
    graph.AddEdge(3, 0, 21);graph.AddEdge(3, 1, 17);graph.AddEdge(3, 2, 16);

    Prim<WeightedDirectedGraphAsAdjMatrix<int>, int> MST(&graph);
    MST.Compute();
    ASSERT_EQ(28, MST.TotalWeight());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
