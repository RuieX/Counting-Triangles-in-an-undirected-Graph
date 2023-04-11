#ifndef LEARNING_MASSIVE_DATA_GRAPH_DS_H
#define LEARNING_MASSIVE_DATA_GRAPH_DS_H

#include <vector>

using namespace std;

class GraphMat {
    public:
    static int countTriangles_node_seq(const vector<vector<bool>>& graph);
    static int countTriangles_edge_seq(const vector<vector<bool>>& graph);

    static int countTriangles_node_multi(const vector<vector<bool>>& graph, int nThreads);
    static int countTriangles_edge_multi(const vector<vector<bool>>& graph, int nThreads);

    static vector<vector<bool>> getGraph_dense(int nNodes, double density);
    static vector<vector<bool>> getGraph(int nNodes, const string& path);

    static vector<vector<int>> precomputeIntersection(const vector<vector<bool>>& graph);
    static int ctTr_edgeFast_seq(const vector<vector<bool>>& graph, const vector<vector<int>>& allInts);
    static int ctTr_edgeFast_multi(const vector<vector<bool>>& graph, int nThreads, const vector<vector<int>>& allInts);

    static int better_algo(const vector<vector<bool>>& graph, const vector<pair<int, int>>& all_edges);

    private:
    static int getIntersection(const vector<bool>& nodeARow, const vector<bool>& nodeBCol, int nodeA, int nodeB);
};

#endif
