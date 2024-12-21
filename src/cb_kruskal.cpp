#define INT_MAX 2147483647

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Edge {
    int u, v, w;
};

struct DisjointSet {
	int p;
	int rank;
};

void buildAdjlist(ifstream& input, vector<vector<Edge>>& adjList, int V, int E, bool directed);
// void findCycle(vector<vector<Edge>>& adjList, vector<Edge>& cycle, int V);
// bool dfs(int v, vector<vector<Edge>>& adjList, vector<bool>& visited, vector<Edge>& cycle, int parent);
// bool dfsReport(int v, vector<vector<Edge>>& adjList, vector<bool>& visited, vector<Edge>& cycle, int parent);
// void reportCycles(vector<vector<Edge>>& adjList, int V);

void initDisjointSets(int V, vector<DisjointSet>& sets);
int findSet(int x, vector<DisjointSet>& sets);
void link(int x, int y, vector<DisjointSet>& sets);

void printAdjList(const vector<vector<Edge>>& adjList);		//debug


int main (int argc, char* argv []) {
	char temp = ' ';
	bool directed = false;
	int V = 0;
	int E = 0;

	ifstream input;
    ofstream output;
    input.open(argv[1]);
    output.open(argv[2]);

	input >> temp >> V >> E;
	directed = (temp == 'd') ? true : false;

	vector<vector<Edge>> adjList(V);
	vector<int> shortestPath(V, INT_MAX);
	vector<Edge> cycle;
	buildAdjlist(input, adjList, V, E, directed);
	printAdjList(adjList);		//debug
//	findCycle(adjList, cycle, V);
//	reportCycles(adjList, V);
	input.close();
	output.close();

	return 0;

}



void buildAdjlist(ifstream& input, vector<vector<Edge>>& adjList, int V, int E, bool directed) {
	for (int i = 0; i < E; ++i) {
		int u, v, w;
		input >> u >> v >> w;

		if (directed) {
			adjList[u].push_back({u, v, w});
		}

		else if (!directed) {
			adjList[u].push_back({u, v, w});
			adjList[v].push_back({v, u, w});
		}
	}
}


/* ------------Debug Functions------------- */
void printAdjList(const vector<vector<Edge>>& adjList) {
	for (int i = 0; i < adjList.size(); ++i) {
		cout << i << ": ";
		for (const auto& edge : adjList[i]) {
			cout << "(" << edge.u << ", " << edge.v << ", " << edge.w << ") ";
		}
		cout << endl;
	}
}
/*
void findCycle(vector<vector<Edge>>& adjList, vector<Edge>& cycle, int V) {
	vector<bool> visited(V, false);
	for (int i = 0; i < V; ++i) {
		if (!visited[i]) {
			if (dfs(i, adjList, visited, cycle, -1)) {
				return;
			}
		}
	}
}

bool dfs(int v, vector<vector<Edge>>& adjList, vector<bool>& visited, vector<Edge>& cycle, int parent) {
	visited[v] = true;
	for (const auto& edge : adjList[v]) {
		if (!visited[edge.v]) {
			cycle.push_back(edge);
			if (dfs(edge.v, adjList, visited, cycle, v)) {
				return true;
			}
			cycle.pop_back();
		} else if (edge.v != parent) {
			cycle.push_back(edge);
			return true;
		}
	}
	return false;
}

void reportCycles(vector<vector<Edge>>& adjList, int V) {
	vector<bool> visited(V, false);
	vector<Edge> cycle;
	for (int i = 0; i < V; ++i) {
		if (!visited[i]) {
			if (dfsReport(i, adjList, visited, cycle, -1)) {
				cout << "Cycle found: ";
				for (const auto& edge : cycle) {
					cout << "(" << edge.u << ", " << edge.v << ", " << edge.w << ") ";
				}
				cout << endl;
				cycle.clear();
			}
		}
	}
}

bool dfsReport(int v, vector<vector<Edge>>& adjList, vector<bool>& visited, vector<Edge>& cycle, int parent) {
	visited[v] = true;
	for (const auto& edge : adjList[v]) {
		if (!visited[edge.v]) {
			cycle.push_back(edge);
			if (dfsReport(edge.v, adjList, visited, cycle, v)) {
				return true;
			}
			cycle.pop_back();
		} else if (edge.v != parent) {
			cycle.push_back(edge);
			return true;
		}
	}
	return false;
}
*/
void initDisjointSets(int V, vector<DisjointSet>& sets) {
	for (int i = 0; i < V; ++i) {
		sets.push_back({i, 0});
	}
}

void link(int x, int y, vector<DisjointSet>& sets) {
	if (sets[x].rank > sets[y].rank) {
		sets[y].p = x;
	} else {
		sets[x].p = y;
		if (sets[x].rank == sets[y].rank) {
			sets[y].rank = sets[y].rank + 1;
		}
	}
}

int findSet(int x, vector<DisjointSet>& sets) {
	if (x != sets[x].p) {
		sets[x].p = findSet(sets[x].p, sets);
	}
	return sets[x].p;
}

