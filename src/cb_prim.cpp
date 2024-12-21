#define INT_MIN -2147483648

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <sstream>

using namespace std;

struct Vertex {
	int key;
	int p;
	int index;
	bool inTree;
};

struct compareVertex {
	bool operator() (const Vertex* v1, const Vertex* v2) {
		return (*v1).key < (*v2).key;
	}
};

struct Edge {
    int u, v, w;
	bool inTree;
};

struct DisjointSet {
	int p;
	int rank;
};

void buildAdjlist(ifstream& input, vector<vector<Edge>>& adjList, int V, int E, bool directed);
void initVertices(vector<Vertex>& vertices, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, int V);
void printAdjList(const vector<vector<Edge>>& adjList);		//debug
void prim(vector<vector<Edge>>& adjList, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, vector<Vertex>& vertices, vector<Edge>& MaxST);
void printMaxST(vector<Edge>& MaxST);
int searchParentIndex(const vector<vector<Edge>>& adjList, int u, int parent);
void notInMaxST(vector<vector<Edge>>& adjList, vector<Edge>& MaxST, ofstream& output);

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
	vector<Edge> cycle;
	priority_queue<Vertex*, vector<Vertex*>, compareVertex> maxHeap;
	vector<Vertex> vertices(V);
	vector<Edge> MaxST;
	
	// if undirected follow this procedure
	initVertices(vertices, maxHeap,  V);
	buildAdjlist(input, adjList, V, E, directed);
	printAdjList(adjList);		//debug
	prim(adjList, maxHeap, vertices, MaxST);
	printMaxST(MaxST);
	notInMaxST(adjList, MaxST, output);
	input.close();
	output.close();

	// if directed follow this procedure
	

	return 0;

}



void buildAdjlist(ifstream& input, vector<vector<Edge>>& adjList, int V, int E, bool directed) {
	for (int i = 0; i < E; ++i) {
		int u, v, w;
		input >> u >> v >> w;

		if (directed) {
			adjList[u].push_back({u, v, w, false});
		}

		else if (!directed) {
			adjList[u].push_back({u, v, w, false});
			adjList[v].push_back({v, u, w, false});
		}
	}
}


void initVertices(vector<Vertex>& vertices, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, int V) {	//initialize and enqueue vertices
	for (int i = 0; i < V; i++) {
		vertices[i].key = INT_MIN;
		vertices[i].p = -1;
		vertices[i].index = i;
		vertices[i].inTree = false;
	}
	maxHeap.push(&vertices[0]);
}

void prim(vector<vector<Edge>>& adjList, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, vector<Vertex>& vertices, vector<Edge>& MaxST) {
	bool first = true;
	int tempIndex = 0;
	int tempParent = 0;
	int iteration = 0;

	while(!maxHeap.empty()) {
		Vertex* u = maxHeap.top();
		maxHeap.pop();

		if(first)	(*u).key = 0;
		else {	// not first -> must have a parent
			int index = searchParentIndex(adjList, (*u).index, (*u).p);
			if(!adjList[(*u).index][index].inTree)	MaxST.push_back(adjList[(*u).index][index]);
			
			adjList[(*u).index][index].inTree = true;
			vertices[adjList[(*u).index][index].v].inTree = true;
			// Search for adjList[u.index][u.p]'s counterpart
			for (int j = 0; j < adjList[(*u).p].size(); j++) {
				if (adjList[(*u).p][j].v == (*u).index) {
					adjList[(*u).p][j].inTree = true;
					break;
				}
			}
		}

		first = false;
		(*u).inTree = true;
		for (int i = 0; i < adjList[(*u).index].size(); i++) {
			Vertex* v = &vertices[adjList[(*u).index][i].v];
			if (!(*v).inTree && adjList[(*u).index][i].w > (*v).key) {
				(*v).p = (*u).index;
				(*v).key = adjList[(*u).index][i].w;	
				maxHeap.push(v);
			}
		}
	}

}




int searchParentIndex(const vector<vector<Edge>>& adjList, int u, int parent) {
	for (int i = 0; i < adjList[u].size(); i++) {
		if (adjList[u][i].v == parent) {
			cout << "Parent index: " << i << endl;
			return i;
		}
	}
}

void notInMaxST(vector<vector<Edge>>& adjList, vector<Edge>& MaxST, ofstream& output) {
	ostringstream unseleceted;
	int removedWeight = 0;
	bool edgeRemoved = false;
	for (int i = 0; i < adjList.size(); i++) {
		for (int j = 0; j < adjList[i].size(); j++) {
			if (!adjList[i][j].inTree) {
				adjList[i][j].inTree = true;	// Use .inTree to mark the edge already checked
				adjList[adjList[i][j].v][searchParentIndex(adjList, adjList[i][j].v, i)].inTree = true;	// Use .inTree to avoid duplicate edges
				edgeRemoved = true;
				unseleceted << adjList[i][j].u << " " << adjList[i][j].v << " " << adjList[i][j].w << endl;
				removedWeight += adjList[i][j].w;
			}
		}
	}

	if (!edgeRemoved) {
		output << 0 << endl;
	} else {
		output << removedWeight << endl;
		output << unseleceted.str() << endl;
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


void printMaxST(vector<Edge>& MaxST) {
	cout << "MaxST size: " << MaxST.size() << endl;
	for (const auto& edge : MaxST) {
		cout << "(" << edge.u << ", " << edge.v << ", " << edge.w << ") " << endl;
	}
	cout << endl;
}
