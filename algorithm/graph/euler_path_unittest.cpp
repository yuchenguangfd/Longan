/*
 * euler_path_unittest.cpp
 * Created on: Jan 13, 2015
 * Author: chenguangyu
 */

#include "euler_path.h"
#include "common/lang/defines.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(EulerPathTest, DirectedGraphFindPathOK) {
    int N = 7;
    int edges[][2] = {{1, 0}, {0, 2}, {2, 3}, {3, 5}, {5, 4}, {4, 2}, {2, 1}, {1, 0}, {0, 6}};
    DirectedGraphAsAdjMatrix<> graph(N);
    for (int i = 0; i < arraysize(edges); ++i) {
        graph.AddEdge(edges[i][0], edges[i][1]);
    }
    EulerPath eulerpath;
    std::vector<int> path;
    bool found = eulerpath.FindEulerPath(graph, 1, 6, &path);
    EXPECT_TRUE(found);
    int ans[] = {1, 0, 2, 3, 5, 4, 2, 1, 0, 6};
    for (int i = 0; i < path.size(); ++i) {
        EXPECT_EQ(ans[i], path[i]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



