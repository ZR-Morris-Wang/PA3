#define INT_MIN -2147483648

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <sstream>

using namespace std;

bool cycleFound = true;

enum Color {WHITE, GRAY, BLACK};

struct Vertex {
	int key;
	int p;
	int index;
	bool inTree;
	Color color;
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
void notInMaxSTDirected(vector<vector<Edge>>& adjList, vector<Edge>& MaxST, ofstream& output);
void breakMinEdge(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int cycleHead, int cycleTail);
void dfsInit(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int V);
void dfsVisit(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int u);
void dfsVisit(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int u, vector<Edge>& MaxST);
void dfs(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int V);
void dfs(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int V, vector<Edge>& MaxST);
void buildDirectedMaxST(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, vector<Edge>& MaxST, int V);
void allPathSetTrue(vector<vector<Edge>>& adjList);


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
	if(!directed) {
		initVertices(vertices, maxHeap,  V);
		buildAdjlist(input, adjList, V, E, directed);
		prim(adjList, maxHeap, vertices, MaxST);
		notInMaxST(adjList, MaxST, output);
	} else {
		
		buildAdjlist(input, adjList, V, E, directed);
		allPathSetTrue(adjList);
		int iteration = 0;
		while(cycleFound) {
			iteration++;
			cout << "Iteration: " << iteration << "\n" << endl;
			dfs(adjList, vertices, V);
		}
		buildDirectedMaxST(adjList, vertices, MaxST, V);
		notInMaxSTDirected(adjList, MaxST, output);
	}

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
		vertices[i].color = WHITE;
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
			// cout << "Parent index: " << i << endl;
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

void notInMaxSTDirected(vector<vector<Edge>>& adjList, vector<Edge>& MaxST, ofstream& output) {
	ostringstream unseleceted;
	int removedWeight = 0;
	bool edgeRemoved = false;
	for (int i = 0; i < adjList.size(); i++) {
		for (int j = 0; j < adjList[i].size(); j++) {
			if (!adjList[i][j].inTree) {
				adjList[i][j].inTree = true;	// Use .inTree to mark the edge already checked
				// adjList[adjList[i][j].v][searchParentIndex(adjList, adjList[i][j].v, i)].inTree = true;	// Use .inTree to avoid duplicate edges
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

void allPathSetTrue(vector<vector<Edge>>& adjList) {
	for (int i = 0; i < adjList.size(); i++) {
		for (int j = 0; j < adjList[i].size(); j++) {
			adjList[i][j].inTree = true;
		}
	}
}

void dfsInit(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int V) {
	for (int i = 0; i < V; i++) {
		vertices[i].key = INT_MIN;		// unused here
		vertices[i].p = -1;
		vertices[i].index = i;
		vertices[i].inTree = false;		// unused here
		vertices[i].color = WHITE;
	}
}

void dfsVisit(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int u) {
	vertices[u].color = GRAY;
	for (int i = 0; i < adjList[u].size(); i++) {
		cout << "u = " << u << endl;
		cout << "u color: " << vertices[adjList[u][i].u].color << endl;
		cout << "v = " << adjList[u][i].v << endl;
		cout << "v color: " << vertices[adjList[u][i].v].color << endl;
		cout << endl;
		if (vertices[adjList[u][i].v].color == WHITE && adjList[u][i].inTree) {
			cycleFound = false;
			vertices[adjList[u][i].v].p = u;
			adjList[u][i].inTree = true;
			if(!cycleFound && vertices[adjList[u][i].v].color != BLACK) {
				cout << "Row 237: " << endl;
				dfsVisit(adjList, vertices, adjList[u][i].v);
			} else {
				return;
			}
		} else if (vertices[adjList[u][i].v].color == GRAY && adjList[u][i].inTree) {
			adjList[u][i].inTree = true;
			cout << "---------------- Cycle Break ----------------" << endl;
			cout << "cycle head: " << adjList[u][i].v << " cycle tail: " << u << endl;
			cycleFound = true;
			breakMinEdge(adjList, vertices, adjList[u][i].v, u);		// breakMinEdge (adjList, cycle head, cycle tail)
			return;
		} else {
			cout << "\nPair skipped\n" << endl;
			continue;
		}

		if(cycleFound) {
			return;
		}
	}
	vertices[u].color = BLACK;
	cout << endl;
	cout << "u = " << u << " color: " << vertices[u].color << endl;
	cout << "\n\n" << endl;
	cycleFound = false;
	return;
}

void dfsVisit(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int u, vector<Edge>& MaxST) {
	vertices[u].color = GRAY;
	for (int i = 0; i < adjList[u].size(); i++) {
		if (vertices[adjList[u][i].v].color == WHITE && adjList[u][i].inTree) {
			cycleFound = false;
			vertices[adjList[u][i].v].p = u;
			adjList[u][i].inTree = true;
			MaxST.push_back(adjList[u][i]);
			if(!cycleFound && vertices[adjList[u][i].v].color != BLACK) {
				dfsVisit(adjList, vertices, adjList[u][i].v);
			} else {
				return;
			}
		} else {		// The cycles should have all been broken
			cout << "\nPair skipped\n" << endl;
			continue;
		}

		if(cycleFound) {
			return;
		}
	}
	vertices[u].color = BLACK;
	cycleFound = false;
	return;
}

void dfs(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int V) {
	dfsInit(adjList, vertices, V);
	int root = rand() % V;
	cout << "Root: " << root << endl;
	cout << "Row 301: " << endl;
	if(vertices[root].color != BLACK)	dfsVisit(adjList, vertices, root/*, time*/);
	if(!cycleFound) {
		for (int i = 0; i < V; i++) {
			if (vertices[i].color == WHITE) {
				cout << "DFS Visit: " << i << endl;
				cout << "Row 307: " << endl;
				dfsVisit(adjList, vertices, i);
				if(cycleFound) {
					return;
				}
			}
		}
	}
	cycleFound = false;
}

void dfs(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int V, vector<Edge>& MaxST){
	cout << "me" << endl;
	dfsInit(adjList, vertices, V);
	for (int i = 0; i < V; i++) {
		if (vertices[i].color == WHITE) {
			MaxST.push_back(adjList[i][vertices[i].p]);
			dfsVisit(adjList, vertices, i);
		}
	}
	cout << "Last traversal done" << endl;
}

void breakMinEdge(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, int cycleHead, int cycleTail) {
	int u = cycleTail;
	int v = searchParentIndex(adjList, cycleTail, cycleHead);
	int minWeight = adjList[u][v].w;
	Edge minEdge = adjList[u][v];
	do {
		cout << "u = " << u << endl;
		cout << "u color: " << vertices[adjList[u][v].u].color << endl;
		cout << "v = " << adjList[u][v].v << endl;
		cout << "v color: " << vertices[adjList[u][v].v].color << endl;
		int temp;
		temp = u;
		u = vertices[adjList[u][v].u].p;
		v = searchParentIndex(adjList, u, temp);
		if(adjList[u][v].w < minWeight) {
			minWeight = adjList[u][v].w;
			minEdge = adjList[u][v];
		}
	} while(adjList[u][v].u != cycleHead);
	
	cout << "\nadjList[u][v].u: " << adjList[u][v].u << " cycleHead: " << cycleHead << endl;
	cout << "minWeight: " << minWeight << endl;
	
	adjList[minEdge.u][searchParentIndex(adjList, minEdge.u, minEdge.v)].inTree = false;
	cout << "removed Edge: " << minEdge.u << " " << minEdge.v << " " << minEdge.w << endl;
	cout << "--------------------------------------------------\n" << endl;
	// dfsVisit(adjList, vertices, minEdge.u);
	

}

void buildDirectedMaxST(vector<vector<Edge>>& adjList, vector<Vertex>& vertices, vector<Edge>& MaxST, int V) {
	dfs(adjList, vertices, V, MaxST);
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
