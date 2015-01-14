/*
 * euler_path.h
 * Created on: Jan 13, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GRAPH_EULER_PATH_H
#define ALGORITHM_GRAPH_EULER_PATH_H

#include "graph.h"
#include <vector>

namespace longan {

class EulerPath {
public:
    bool FindEulerPath(DirectedGraphAsAdjMatrix<>& graph , int startVertex, int finishVertex,
            std::vector<int> *path) {
        for (int i = 0; i < graph.NumVertex(); ++i) {
            int in = graph.DegreeInOfVertex(i);
            int out = graph.DegreeOutOfVertex(i);
            if (in != out) {
                if (i == startVertex && out == in + 1) continue;
                if (i == finishVertex && in == out + 1) continue;
                return false;
            }
        }
        path->clear();
        Find(startVertex, graph, path);
        std::reverse(path->begin(), path->end());
        return true;
    }
private:
    void Find(int u, DirectedGraphAsAdjMatrix<>& graph, std::vector<int> *path) {
        for (int v = 0; v < graph.NumVertex(); ++v) {
            if (graph.ExistEdge(u, v)) {
                graph.RemoveEdge(u, v);
                Find(v, graph, path);
            }
        }
        path->push_back(u);
    }
};

} //~ namespace longan

#endif /* ALGORITHM_GRAPH_EULER_PATH_H */
