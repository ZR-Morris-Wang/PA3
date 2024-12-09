#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Edge {
    int u, v, w;
};

void buildAdjlist(ifstream& input, vector<vector<Edge>>& adjList, int V, int E, bool directed);

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
	buildAdjlist(input, adjList, V, E, directed);


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