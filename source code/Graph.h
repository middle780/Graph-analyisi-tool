#pragma once
#include <vector>
#include <string>
using namespace std;

class Graph {
private:
    vector<vector<int>> adjacencyMatrix;
    int numVertices;

public:
    Graph(int vertices);
    void addEdge(int source, int dest, int weight);
    vector<vector<int>> getMST();
    vector<int> getShortestPath(int start, int end);
    bool loadFromFile(const string& filename);
    bool saveToFile(const string& filename, const vector<vector<int>>& result);
    int getNumVertices() const { return numVertices; }
    int getEdgeWeight(int source, int dest) const {
        return adjacencyMatrix[source][dest];
    }
};
