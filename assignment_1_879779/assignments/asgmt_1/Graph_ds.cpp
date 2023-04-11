#include "Graph_ds.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <fstream>

using namespace std;

/**
 * Count the number of triangles in an undirected graph
 * Iterate over all possible combinations of three vertices in the graph.
 * For each triple of vertices (i, j, k), check if there are edges between all three vertices.
 * @param graph
 * @return No. of triangles in graph
 */
int GraphMat::countTriangles_node_seq(const vector<vector<bool>>& graph) {
    int count = 0;

    // Loop through all possible combinations of nodes
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i + 1; j < graph.size(); j++) {
            if (graph[i][j]) {
                for (int k = j + 1; k < graph.size(); k++) {
                    // Check if the three vertices form a triangle
                    if (graph[j][k] && graph[k][i]) {
                        count++;
                    }
                }
            }
        }
    }
    // Return the total count of triangles
    return count;
}

/**
 * For-each edge e=(u,v) in the graph G:
 * TRIANGLES += N(u) ∩ N(v)
 * TRIANGLES /= 3
 * @param graph
 * @return No. of triangles in graph
 */
int GraphMat::countTriangles_edge_seq(const vector<vector<bool>>& graph) {
    int count = 0;

    // Loop through all edges
    for (int i = 0; i < graph.size(); i++)
        for (int j = i + 1; j < graph.size(); j++)
            if (graph[i][j]){
                count += getIntersection(graph[i], graph[j], i, j);
            }
    // Return the total count of triangles/3, to subtract duplicates
    return count/3;
}

/**
 * Get the number of neighboring nodes in common between two nodes.
 * For each neighbor in common, there exist a triangle formed with (nodeA, nodeB, neighbor)
 * @param nodeARow
 * @param nodeBCol
 * @param nodeA
 * @param nodeB
 * @return No. of neighboring nodes in common between the two nodes excluding nodeA and nodeB
 */
int GraphMat::getIntersection(const vector<bool>& nodeARow, const vector<bool>& nodeBCol, int nodeA, int nodeB) {
    int count = 0;

    for (int i = 0; i < nodeARow.size(); i++) {
        if (nodeARow[i] && nodeBCol[i] && i != nodeA && i != nodeB) { //+1 only if there is an edge
            count++;
        }
    }
    return count;
}

//    Note that this approach has limitations for large graphs, as the number of iterations can be very large,
//    and the overhead of creating and synchronizing threads may become significant.
//    Therefore, it is important to carefully consider the size of the input graph and the number of threads used.
// It is best to use dynamic scheduling here because each iteration has a different computational weight schedule
/**
 * Parallelized version of countTriangles_node_seq
 * @param graph
 * @param nThreads
 * @return
 */
int GraphMat::countTriangles_node_multi(const vector<vector<bool>>& graph, int nThreads) {
    int count = 0;

    // Loop through all possible combinations of nodes in parallel
    #pragma omp parallel for num_threads(nThreads) reduction(+:count) schedule(dynamic) shared(graph) default(none)
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i + 1; j < graph.size(); j++) {
            if (graph[i][j]) {
                for (int k = j + 1; k < graph.size(); k++) {
                    // Check if the three vertices form a triangle
                    if (graph[j][k] && graph[k][i]) {
                        count++;
                    }
                }
            }
        }
    }
    // Return the total count of triangles
    return count;
}

// It is best to use dynamic scheduling here because each iteration has a different computational weight schedule
//    Note that the getIntersection function is not parallelized, so it will be called sequentially.
//    Depending on the size of the input graph and the number of threads used, the performance improvement may vary.
//    The reduction(+: count) clause ensures that the count variable is properly updated across all threads.
/**
 * Parallelized version of countTriangles_edge_seq
 * @param graph
 * @param nThreads
 * @return
 */
int GraphMat::countTriangles_edge_multi(const vector<vector<bool>>& graph, int nThreads) {
    int count = 0;

    // Loop through all edges in parallel
    #pragma omp parallel for num_threads(nThreads) reduction(+:count) schedule(dynamic) shared(graph) default(none)
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i + 1; j < graph.size(); j++) {
            if (graph[i][j]) {
                count += getIntersection(graph[i], graph[j], i, j);
            }
        }
    }
    // Return the total count of triangles/3, to subtract duplicates
    return count / 3;
}

/**
 * Get the graph from a file of edges
 * @param nNodes No. of nodes in the graph
 * @param path Path of the file containing all the edges of the graph
 * @return
 */
vector<vector<bool>> GraphMat::getGraph(int nNodes, const string& path) {
    auto start = chrono::high_resolution_clock::now();
    // Read graph from file
    ifstream inputFile (path);
    vector<vector<bool>> graph(nNodes, vector<bool>(nNodes, false));
    int nEdges = 0;

    // Read in the edges of the graph
    if ( inputFile.is_open() ) {
        while ( inputFile.good() ) {
            int u, v;
            inputFile >> u >> v;
            graph[u][v] = true;
            graph[v][u] = true;
            nEdges++;
        }
        inputFile.close();
    }

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start);
    cout << "Graph creation: " << elapsed.count() << " ms." << endl;

    long double maxEdges = nNodes * ((long double)nNodes - 1) / 2;
    long double density = nNodes / maxEdges;
    cout << "number of nodes: " << nNodes << ", number of edges: " << nEdges << ", density: " << fixed << setprecision(5) << density << endl;

    return graph;
}

/**
 * Create a graph given number of nodes and density
 * @param nNodes
 * @param density
 * @return
 */
vector<vector<bool>> GraphMat::getGraph_dense(int nNodes, double density) {
    auto start = chrono::high_resolution_clock::now();
    vector<vector<bool>> graph(nNodes, vector<bool>(nNodes, false));
    int nEdges = 0;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0, 1);

    for (int i = 0; i < nNodes; i++) {
        for (int j = i + 1; j < nNodes; j++) {
            double r = dis(gen);
            if (r < density) {  // Set edge with probability "density"
                graph[i][j] = true;
                graph[j][i] = true;
                nEdges++;
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start);
    cout << "Graph creation: " << elapsed.count() << " ms." << endl;

    long double maxEdges = nNodes*((long double)nNodes-1)/2;
    long double actual_density = nEdges / maxEdges;
    cout << "number of nodes: " << nNodes << ", number of edges: " << nEdges << ", density: " << fixed << setprecision(5) << actual_density << endl;

    return graph;
}

//Extra part not delivered by deadline

/**
 * Precompute the intersection for all couple of node
 * @param graph
 * @return
 */
vector<vector<int>> GraphMat::precomputeIntersection(const vector<vector<bool>> &graph) {
    auto start = chrono::high_resolution_clock::now();
    vector<vector<int>> allInts(graph.size(), vector<int>(graph.size()));

    #pragma omp parallel for num_threads(2) schedule(dynamic) shared(graph, allInts) default(none)
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i+1; j < graph.size(); j++) {
            int count = 0;
            for (int z = 0; z < graph.size(); z++) {
                if (graph[i][z] && graph[j][z] && z != i && z != j) { //+1 only if there is an edge
                    count++;
                }
            }
            allInts[i][j] = count;
            allInts[j][i] = count;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start);
    cout << "All intersection computation: " << elapsed.count() << " ms." << endl;
    return allInts;
}

/**
 * Same sequential algorithm but with constant time for intersection
 * @param graph
 * @return
 */
int GraphMat::ctTr_edgeFast_seq(const vector<vector<bool>> &graph, const vector<vector<int>>& allInts) {
    int count = 0;
    // Loop through all edges
    for (int i = 0; i < graph.size(); i++)
        for (int j = i + 1; j < graph.size(); j++)
            if (graph[i][j]){
                count += allInts[i][j];
            }
    // Return the total count of triangles/3, to subtract duplicates
    return count/3;
}

/**
 * Same parallelized algorithm but with constant time for intersection
 * @param graph
 * @param nThreads
 * @return
 */
int GraphMat::ctTr_edgeFast_multi(const vector<vector<bool>> &graph, int nThreads, const vector<vector<int>>& allInts) {
    int count = 0;

    // Loop through all edges in parallel
    #pragma omp parallel for num_threads(nThreads) reduction(+:count) schedule(dynamic) shared(graph, allInts) default(none)
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i + 1; j < graph.size(); j++) {
            if (graph[i][j]) {
                count += allInts[i][j];
            }
        }
    }
    // Return the total count of triangles/3, to subtract duplicates
    return count / 3;
}



//TODO: Why this algorithm is bad.
// instead of checking if an edge exists, keep a list of all edges, SORT them by (nodeA Id, nodeB Id), and do the thing for each pair of node,
// this way we skip all the 0s, this is useful especially if we have large sparse graph
// we still need the adjacency matrix for the intersection (in the end it will be the same thing)
// complexity O(#edges * #nodes), which in worst case would still be n^3 but at least we don't check useless cells
int GraphMat::better_algo(const vector<vector<bool>>& graph, const vector<pair<int, int>>& all_edges){
    int count = 0;

    // Loop through all existent edges
    for (auto [x,y]: all_edges) {
        count += getIntersection(graph[x], graph[y], x, y);
    }// since each edge is repeated only once (if A-B, then there is no B-A in the list), there is no need to sort the list and check if x<y

    // Return the total count of triangles/3, to subtract duplicates
    return count/3;
}
//TODO: it should work but i need to get the list of edges as pair of int int first (while i create the adjacency matrix)