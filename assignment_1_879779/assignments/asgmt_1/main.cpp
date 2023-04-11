#include "Graph_ds.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <numeric>
#include <omp.h>

using namespace std;

#define N_THREADS 20 //max thread to use
#define N_ITERATION 2 //test iterations

/**
 *
 * @param option dataset selection
 * @return data related to the selected dataset
 */
auto getData(int option){
    struct graphData {int numNodes; int numEdges; int realTriangles; string inputFile;};

    switch (option) {
        case 1:
            // email-Eu-core network 105461 triangles
            // numNodes = 1005, numEdges = 25571 ,"../assignments/asgmt_1/data/email-Eu-core.txt";
            return graphData {1005, 25571, 105461,
                              "../assignments/asgmt_1/data/email-Eu-core.txt"};
        case 2:
            // Social circles: Facebook 1612010 triangles
            // numNodes = 4039, numEdges = 88234, "../assignments/asgmt_1/data/facebook_combined.txt";
            return graphData {4039, 88234, 1612010,
                              "../assignments/asgmt_1/data/facebook_combined.txt"};
        case 3:
            // Enron email network 727044 triangles
            // numNodes = 36692, numEdges = 183831, "../assignments/asgmt_1/data/Email-Enron.txt";
            return graphData {36692, 183831, 727044,
                              "../assignments/asgmt_1/data/Email-Enron.txt"};
        case 4:
            // Brightkite 494728 triangles
            // numNodes = 58228, numEdges = 214078, "../assignments/asgmt_1/data/Brightkite_edges.txt";
            return graphData {58228, 214078, 494728,
                              "../assignments/asgmt_1/data/Brightkite_edges.txt"};
        default:
            throw std::invalid_argument("Invalid selection");
    }
}

/**
 * Run the algorithm with either the algorithm that count triangles through edges and divide by 3,
 * or the algorithm that checks the triple of nodes.
 * @param nThreads number of threads that need to be used to run the algorithm
 * @param graph
 * @param execution_times number of times to execute the algorithm
 */
void test(int nThreads, const vector<vector<bool>>& graph, vector<vector<double>>& execution_times) {
    for (int j = 0; j < N_ITERATION; j++) {
        cout << "\n" << "--number of threads: " << nThreads << ", " << "iteration: " << j+1 << "--" << endl;
        auto start = chrono::high_resolution_clock::now();

        int numTriangles;
        if(nThreads == 1) {
            numTriangles = GraphMat::countTriangles_edge_seq(graph);
//            numTriangles = GraphMat::countTriangles_node_seq(graph);
        } else {
            numTriangles = GraphMat::countTriangles_edge_multi(graph, nThreads);
//            numTriangles = GraphMat::countTriangles_node_multi(graph, nThreads);
        }

        auto end = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start);

        // Print the total count of triangles
        cout << "Total number of triangles in the graph: " << numTriangles << endl;
        cout << "Elapsed: " << elapsed.count() << " ms." << endl;

        execution_times[nThreads - 1][j] = elapsed.count(); //row = nThreads, column = i-th execution
    }
}

/**
 * Run the algorithm that count triangles through edges and divide by 3 but with precomputed intersection
 * @param nThreads number of threads that need to be used to run the algorithm
 * @param graph
 * @param execution_times number of times to execute the algorithm
 * @param allInts matrix with all the intersection precomputed
 */
void test_fast(int nThreads, const vector<vector<bool>>& graph, vector<vector<double>>& execution_times, vector<vector<int>>& allInts) {
    for (int j = 0; j < N_ITERATION; j++) {
        cout << "\n" << "--number of threads: " << nThreads << ", " << "iteration: " << j+1 << "--" << endl;
        auto start = chrono::high_resolution_clock::now();
        int numTriangles;
        if(nThreads == 1) {
            numTriangles = GraphMat::ctTr_edgeFast_seq(graph, allInts);
        } else {
            numTriangles = GraphMat::ctTr_edgeFast_multi(graph, nThreads, allInts);
        }
        auto end = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds> (end - start);

        // Print the total count of triangles
        cout << "Total number of triangles in the graph: " << numTriangles << endl;
        cout << "Elapsed: " << elapsed.count() << " ms." << endl;
        execution_times[nThreads - 1][j] = elapsed.count(); //row = nThreads, column = i-th execution
    }
}

/**
 * Save the execution times for each number of threads used to run the algorithm on a graph in a .CSV file
 * @param execTimes 2D vector that contains all the execution times
 * @param name
 */
void saveExecutionTimes(const vector<vector<double>>& execTimes, const string& name) {
    stringstream filename;
    filename << "../assignments/asgmt_1/execution_times/results_" << name << ".csv";

    // Open and write the execution times to the CSV file
    ofstream outFile(filename.str(), ios::trunc);
    if (outFile.is_open()) {
        for (const auto& row : execTimes) {
            for (const auto& time : row) {
                outFile << time << ",";
            }
            outFile << endl;
        }
        outFile.close();
    } else {
        cerr << "Error: unable to open output file." << endl;
        return;
    }
}

int main() {
    // Setup for generated graphs and files' name
    vector<int> numNod = {5000, 10000, 70000, 1000};
    vector<double> numDens = {0.5, 0.05, 0.00005, 0.95};
//        vector<string> fileNames = {"1005_email","4039_fb","36692_enron","58228_bright","5000_50","10000_05","70000_00005", "1000_95"};
//        vector<string> fileNames = {"node_1005_email","node_4039_fb","node_36692_enron","node_58228_bright","node_5000_50","node_10000_05","node_70000_00005", "node_1000_95"};
    vector<string> fileNames = {"fast_1005_email","fast_4039_fb","fast_36692_enron","fast_58228_bright","fast_5000_50","fast_10000_05","fast_70000_00005", "fast_1000_95"};

    // Run the algorithm on all graphs and save the execution times
    for (int dataNum = 2; dataNum < 9; dataNum++) {
        vector<vector<bool>> graph;
        if(dataNum < 5) {
            // get dataset information
            auto [numNodes, numEdges, realTriangles, inputFile] = getData(dataNum);
            // create an adjacency matrix to represent the graph
            graph = GraphMat::getGraph(numNodes, inputFile);
        } else {
            // generate dense graph adjacency matrix by passing number of nodes and density
            graph = GraphMat::getGraph_dense(numNod[dataNum-5],numDens[dataNum-5]);
        }
        cout << "Max number of threads: " << omp_get_max_threads() << endl;

        // Create a matrix that contains all the intersection count
//        auto allInts = GraphMat::precomputeIntersection(graph);

        // Create 2D vector to store execution times
        vector<vector<double>> execution_times(N_THREADS, vector<double>(N_ITERATION));
        // Count triangles
        for (int i = 1; i <= N_THREADS; i++) {
            test(i, graph, execution_times);
//            test_fast(i, graph, execution_times, allInts);
        }
        saveExecutionTimes(execution_times, fileNames[dataNum-1]);
    }
    return 0;
}