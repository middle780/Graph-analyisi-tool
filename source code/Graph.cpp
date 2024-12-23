#include "Graph.h"
#include <fstream>
#include <queue>
#include <limits>
using namespace std;

Graph::Graph(int vertices) : numVertices(vertices) {
    adjacencyMatrix.resize(vertices, vector<int>(vertices, 0));
}

void Graph::addEdge(int source, int dest, int weight) {
    adjacencyMatrix[source][dest] = weight;
    adjacencyMatrix[dest][source] = weight; // Undirected graph
}

bool Graph::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    if (!(file >> numVertices) || numVertices <= 0) return false;

    adjacencyMatrix.clear();
    adjacencyMatrix.resize(numVertices, vector<int>(numVertices));

    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            file >> adjacencyMatrix[i][j];
        }
    }

    file.close();
    return true;
}

bool Graph::saveToFile(const string& filename, const vector<vector<int>>& result) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& row : result) {
        for (int val : row) {
            file << val << " ";
        }
        file << "\n";
    }

    file.close();
    return true;
}

vector<vector<int>> Graph::getMST() {
    vector<bool> visited(numVertices, false);
    vector<int> key(numVertices, numeric_limits<int>::max());
    vector<int> parent(numVertices, -1);

    key[0] = 0;

    for (int count = 0; count < numVertices - 1; count++) {
        int minKey = numeric_limits<int>::max();
        int u = -1;

        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && key[v] < minKey) {
                minKey = key[v];
                u = v;
            }
        }

        visited[u] = true;

        for (int v = 0; v < numVertices; v++) {
            if (adjacencyMatrix[u][v] && !visited[v] && adjacencyMatrix[u][v] < key[v]) {
                parent[v] = u;
                key[v] = adjacencyMatrix[u][v];
            }
        }
    }

    vector<vector<int>> result;
    for (int i = 1; i < numVertices; i++) {
        result.push_back({ parent[i], i, adjacencyMatrix[i][parent[i]] });
    }

    return result;
}

vector<int> Graph::getShortestPath(int start, int end) {
    vector<int> dist(numVertices, numeric_limits<int>::max());
    vector<int> prev(numVertices, -1);
    vector<bool> visited(numVertices, false);

    dist[start] = 0;

    for (int count = 0; count < numVertices - 1; count++) {
        int minDist = numeric_limits<int>::max();
        int u = -1;

        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && dist[v] < minDist) {
                minDist = dist[v];
                u = v;
            }
        }

        if (u == -1) break;

        visited[u] = true;

        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && adjacencyMatrix[u][v] &&
                dist[u] != numeric_limits<int>::max() &&
                dist[u] + adjacencyMatrix[u][v] < dist[v]) {
                dist[v] = dist[u] + adjacencyMatrix[u][v];
                prev[v] = u;
            }
        }
    }

    // Reconstruct path
    vector<int> path;
    if (dist[end] == numeric_limits<int>::max()) {
        return path;  // No path exists
    }

    for (int at = end; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return path;
}