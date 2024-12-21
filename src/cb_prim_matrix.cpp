#define INT_MIN -2147483648
// #define DEBUG
// #define DEBUG2

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

struct edgeCopy;

struct Edge {
    int u, v, w;
	bool inTree;

	// AI gen begin
	struct edgeCopy toEdgeCopy() const;
	// AI gen end
};



struct DisjointSet {
	int p;
	int rank;
};

void buildAdjMatrix(ifstream& input, vector<vector<Edge>>& adjMatrix, int V, int E, bool directed);
void initVertices(vector<Vertex>& vertices, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, int V);
void printAdjMatrix(const vector<vector<Edge>>& adjMatrix, int V);		//debug
void prim(vector<vector<Edge>>& adjMatrix, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, vector<Vertex>& vertices, vector<Edge>& MaxST, int V);
void printMaxST(vector<Edge>& MaxST);
// int searchParentIndex(const vector<vector<Edge>>& adjMatrix, int u, int parent);
void notInMaxST(vector<vector<Edge>>& adjMatrix, vector<Edge>& MaxST, ofstream& output, int V);
void notInMaxSTDirected(vector<vector<Edge>>& adjMatrix, vector<Edge>& MaxST, ofstream& output, vector<Vertex>& vertices, int V);
void breakMinEdge(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int cycleHead, int cycleTail);
void dfsInit(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int V);
void dfsVisit(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int u, int V);
void dfsVisit(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int u, vector<Edge>& MaxST, int V);
void dfs(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int V);
void dfs(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int V, vector<Edge>& MaxST);
void buildDirectedMaxST(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, vector<Edge>& MaxST, int V);
void allPathSetTrue(vector<vector<Edge>>& adjMatrix, int V);
void removeRedundantEdge(vector<vector<Edge>>& adjMatrix, int V);

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

	vector<vector<Edge>> adjMatrix(V, vector<Edge>(V, {-1, -1, -101, false}));
	vector<Edge> cycle;
	priority_queue<Vertex*, vector<Vertex*>, compareVertex> maxHeap;
	vector<Vertex> vertices(V);
	vector<Edge> MaxST;
	
	// if undirected follow this procedure
	if(!directed) {
		initVertices(vertices, maxHeap,  V);
		buildAdjMatrix(input, adjMatrix, V, E, directed);
		// printAdjMatrix(adjMatrix, V);		//debug
		prim(adjMatrix, maxHeap, vertices, MaxST, V);
		// printMaxST(MaxST);
		notInMaxST(adjMatrix, MaxST, output, V);
	} else {
		
		buildAdjMatrix(input, adjMatrix, V, E, directed);
		allPathSetTrue(adjMatrix, V);
		int iteration = 0;

		while(cycleFound) {
			iteration++;
			
			#ifdef DEBUG
			cout << "Iteration: " << iteration << "\n" << endl;
			#endif

			dfs(adjMatrix, vertices, V);
			
			#ifdef DEBUG
			cout << "Cycle Found: " << cycleFound << endl;
			#endif
		}

		buildDirectedMaxST(adjMatrix, vertices, MaxST, V);
		// removeRedundantEdge(adjMatrix, V);
		notInMaxSTDirected(adjMatrix, MaxST, output, vertices, V);
	}

	input.close();
	output.close();

	// if directed follow this procedure


	return 0;

}



void buildAdjMatrix(ifstream& input, vector<vector<Edge>>& adjMatrix, int V, int E, bool directed) {
	for(int i = 0; i < V; i++) {
		for(int j = 0; j < V; j++) {
			adjMatrix[i][j] = {-1, -1, -101, false};
		}
	}
	
	for (int i = 0; i < E; ++i) {
		int u, v, w;
		input >> u >> v >> w;

		if (directed) {
			adjMatrix[u][v] = {u, v, w, false};
		}

		else if (!directed) {
			adjMatrix[u][v] = {u, v, w, false};
			adjMatrix[v][u] = {v, u, w, false};
		}
	}

	#ifdef DEBUG2
	cout << "Adjacency Matrix built" << endl;
	#endif
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

void prim(vector<vector<Edge>>& adjMatrix, priority_queue<Vertex*, vector<Vertex*>, compareVertex>& maxHeap, vector<Vertex>& vertices, vector<Edge>& MaxST, int V) {
	bool first = true;

	while(!maxHeap.empty()) {
		Vertex* u = maxHeap.top();
		maxHeap.pop();

		if(first)	(*u).key = 0;
		else {	// not first -> must have a parent
			if(!adjMatrix[(*u).index][vertices[(*u).p].index].inTree)	MaxST.push_back(adjMatrix[(*u).index][vertices[(*u).p].index]);
				
				adjMatrix[(*u).index][vertices[(*u).p].index].inTree = true;	// put edge in tree
				
				// vertices[adjMatrix[(*u).index][vertices[(*u).p].index].v].inTree = true;
				adjMatrix[vertices[(*u).p].index][(*u).index].inTree = true;	// put counterparting edge in tree
			}

		first = false;
		(*u).inTree = true;
		for (int i = 0; i < V; i++) {
			Vertex* v = &vertices[adjMatrix[(*u).index][i].v];
			if (!(*v).inTree && adjMatrix[(*u).index][i].w > (*v).key) {
				(*v).p = (*u).index;
				(*v).key = adjMatrix[(*u).index][i].w;	
				maxHeap.push(v);
			}
		}
	}

}


struct edgeCopy : Edge {
	bool Copied = false;
	Edge toEdge();
};


// AI gen begin
edgeCopy Edge::toEdgeCopy() const {
    edgeCopy copy;
    copy.u = u;
    copy.v = v;
    copy.w = w;
    copy.inTree = inTree;
    copy.Copied = false; // Default value
    return copy;
}


Edge edgeCopy::toEdge() {
	Edge copy;
	copy.u = u;
	copy.v = v;
	copy.w = w;
	copy.inTree = inTree;
	return copy;
}

// AI gen end
void removeRedundantEdge(vector<vector<Edge>>& adjMatrix, int V) {
	int E = 0;

	#ifdef DEBUG2
	printAdjMatrix(adjMatrix, V);
	cout << endl;
	#endif
	
	
	vector<vector<edgeCopy>> adjMatrixCopy(V, vector<edgeCopy>(V));
	
	for (int i = 0; i < V; ++i) {
		for (int j = 0; j < V; ++j) {
			if(adjMatrix[i][j].w != -101 && adjMatrix[i][j].inTree) {
			// cout << adjMatrix[i][j].u << " " << adjMatrix[i][j].v << " " << adjMatrix[i][j].w << " " << adjMatrix[i][j].inTree << endl;
				adjMatrixCopy[i][j].u = adjMatrix[i][j].u;
				adjMatrixCopy[i][j].v = adjMatrix[i][j].v;
				adjMatrixCopy[i][j].w = adjMatrix[i][j].w;
				adjMatrixCopy[i][j].inTree = adjMatrix[i][j].inTree;
				adjMatrixCopy[i][j].Copied = false;
			} else {
				adjMatrixCopy[i][j].u = -1;
				adjMatrixCopy[i][j].v = -1;
				adjMatrixCopy[i][j].w = -101;
				adjMatrixCopy[i][j].inTree = false;
				adjMatrixCopy[i][j].Copied = false;			
			}
		}
	}

	// cout << "After first print" <<endl;

	for(int i = 0; i < V; i++) {
		for(int j = 0; j < V; j++) {
			if(adjMatrix[i][j].inTree == true && !adjMatrixCopy[i][j].Copied) {
				adjMatrixCopy[j][i].u = adjMatrix[i][j].v;
				adjMatrixCopy[j][i].v = adjMatrix[i][j].u;
				adjMatrixCopy[j][i].w = adjMatrix[i][j].w;
				adjMatrixCopy[j][i].inTree = true;
				adjMatrixCopy[j][i].Copied = true;
			}
		}
	}

	// cout << "After first copy" << endl;
// AI gen begin
	#ifdef DEBUG2
	cout << "\n";
	for (int i = 0; i < V; ++i) {
		cout << i << ": ";
		for (int j = 0; j < V; ++j) {
			if (adjMatrixCopy[i][j].w != -101) {
				cout << "(" << adjMatrixCopy[i][j].u << ", " << adjMatrixCopy[i][j].v << ", " << adjMatrixCopy[i][j].w << ", " << adjMatrixCopy[i][j].inTree << ") ";
			}
		}
		cout << endl;
	}
	#endif
	cout << endl;
// AI gen end

	vector<vector<Edge>> adjMatrixRef(V, vector<Edge>(V));
	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			// cout << "(" << adjMatrixCopy[i][j].u << ", " << adjMatrixCopy[i][j].v << ", " << adjMatrixCopy[i][j].w << ") ";
			if(adjMatrixCopy[i][j].w != -101) {
				adjMatrixRef[i][j].u = adjMatrixCopy[i][j].u;
				adjMatrixRef[i][j].v = adjMatrixCopy[i][j].v;
				adjMatrixRef[i][j].w = adjMatrixCopy[i][j].w;
				adjMatrixRef[i][j].inTree = false;
			} else {
				adjMatrixRef[i][j].u = -1;
				adjMatrixRef[i][j].v = -1;
				adjMatrixRef[i][j].w = -101;
				adjMatrixRef[i][j].inTree = false;
			}
		}
	}

	#ifdef DEBUG2
	cout << "\nadjMatrixRef for prim:" << 	endl;
	printAdjMatrix(adjMatrixRef, V);
	cout << "After second print" << endl;
	cout << endl;
	#endif


	vector<Vertex> vertices(V);
	priority_queue<Vertex*, vector<Vertex*>, compareVertex> maxHeap;
	vector<Edge> MaxST;
	initVertices(vertices, maxHeap,  V);
	
	#ifdef DEBUG2
	cout << "\nadjMatrix: " << endl;
	printAdjMatrix(adjMatrix, V);		//debug
	cout << endl;
	#endif

	prim(adjMatrixRef, maxHeap, vertices, MaxST, V);

	#ifdef DEBUG2
	cout << "\nMaxST: " << endl;
	printMaxST(MaxST);
	#endif

	// for(int i = 0; i < MaxST.size(); i++) {
	// 	for(int j = 0; j < V; j++) {
	// 		adjMatrixCopy[i][j].inTree = false;
	// 	}
	// }

	for(auto& edge : MaxST) {
		edge.inTree = !edge.inTree;
	}

	#ifdef DEBUG2
	cout << "\nMaxST: " << endl;
	printMaxST(MaxST);
	#endif 

	for(int i = 0; i < V; i++) {
		for(int j = 0; j < V; j++) {
			adjMatrixCopy[i][j].inTree = false;
		}
	}


	for(int i = 0; i < MaxST.size(); i++) {
		if(!adjMatrix[MaxST[i].u][MaxST[i].v].inTree) {
			cout << "here ";
			adjMatrixCopy[MaxST[i].u][MaxST[i].v].inTree = false;
			adjMatrixCopy[MaxST[i].v][MaxST[i].u].inTree = true;
		} else {
			adjMatrixCopy[MaxST[i].u][MaxST[i].v].inTree = true;
			adjMatrixCopy[MaxST[i].v][MaxST[i].u].inTree = false;
		}
		
	}

	



	#ifdef DEBUG2
	cout << "\nadjMatrixCopy: " << endl;
	for (int i = 0; i < adjMatrixCopy.size(); ++i) {
			cout << i << ": ";
			for (int j = 0; j < adjMatrixCopy[i].size(); ++j) {
				if (adjMatrixCopy[i][j].w != -101) {
					cout << "(" << adjMatrixCopy[i][j].u << ", " << adjMatrixCopy[i][j].v << ", " << adjMatrixCopy[i][j].w;
					#ifdef DEBUG2
					cout << ", " << ((adjMatrixCopy[i][j].inTree) ? "True" : "False") << ", " << ((adjMatrixCopy[i][j].Copied) ? "True" : "False");
					#endif
					cout << ") ";
				}
			}
			cout << endl;
		}
	cout << endl;
	#endif


	for(int i = 0; i < V; i++) {
		for(int j = 0; j < V; j++) {
			if(adjMatrixCopy[i][j].inTree == true) {
				if(adjMatrixCopy[i][j].Copied == true) {
					adjMatrixCopy[i][j] = adjMatrix[i][j].toEdgeCopy();
				}
			} else {
				if(adjMatrixCopy[i][j].Copied == true) {
					adjMatrixCopy[i][j] = adjMatrix[i][j].toEdgeCopy();
				}
			}
		}
	}



	// for(int i = 0; i < V; i++) {
	// 	for(int j = 0; j < V; j++) {
	// 		if(adjMatrixRef[i][j].inTree == true) {
	// 			if(adjMatrixCopy[i][j].inTree == false) {
	// 				adjMatrixCopy[i][j] = adjMatrix[i][j].toEdgeCopy();
	// 			}
	// 		} else {
	// 			if(adjMatrixCopy[i][j].inTree == true) {
	// 				adjMatrixCopy[i][j].inTree = false;
	// 			} else {
	// 				adjMatrixCopy[i][j] = adjMatrix[i][j].toEdgeCopy();
	// 			}
	// 		}
	// 	}
	// }

	vector<vector<Edge>> result(V, vector<Edge>(V));
	for (int i = 0; i < V; ++i) {
		for (int j = 0; j < V; ++j) {
			result[i][j] = adjMatrixCopy[i][j].toEdge();
		}
	}

#ifdef DEBUG2
cout << "\nResult: " << endl;
	printAdjMatrix(result, V);
#endif

	for(int i = 0; i < V; i++) {
		for(int j = 0; j < V; j++) {
			if(result[i][j].inTree == false && result[i][j].w < 0) {
				adjMatrix[i][j].inTree = false;
			} else if(result[i][j].inTree == false && result[i][j].w > 0) {
				adjMatrix[i][j].inTree = true;
			}
		}
		
	}
	// printMaxST(MaxST);
	// int maxNegativeWeight = INT_MIN;
	// int maxNegativeIndex = -1;
	// bool positiveEdgeFound = false;

	// for (int j = 0; j < V; j++) {
	// 	maxNegativeWeight = INT_MIN;
	// 	maxNegativeIndex = -1;
	// 	positiveEdgeFound = false;

	// 	// First pass to find the largest negative weight and check for positive edges
	// 	for (int i = 0; i < V; i++) {
	// 		if (adjMatrix[i][j].w > 0) {
	// 			positiveEdgeFound = true;
	// 			break;
	// 		}
	// 		if (adjMatrix[i][j].w < 0 && adjMatrix[i][j].w > maxNegativeWeight) {
	// 			maxNegativeWeight = adjMatrix[i][j].w;
	// 			maxNegativeIndex = i;
	// 		}
	// 	}

	// 	// Second pass to set inTree property
	// 	for (int i = 0; i < V; i++) {
	// 		if (positiveEdgeFound) {
	// 			if (adjMatrix[i][j].w < 0 && adjMatrix[i][j].w != -101) {
	// 				adjMatrix[i][j].inTree = false;
	// 			}
	// 		} else {
	// 			if (i != maxNegativeIndex && adjMatrix[i][j].w < 0 && adjMatrix[i][j].w != -101) {
	// 				adjMatrix[i][j].inTree = false;
	// 			}
	// 		}
	// 	}
	// }


		
	}







void notInMaxST(vector<vector<Edge>>& adjMatrix, vector<Edge>& MaxST, ofstream& output, int V) {
	ostringstream unseleceted;
	int removedWeight = 0;
	bool edgeRemoved = false;
	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			if (!adjMatrix[i][j].inTree && i != j && adjMatrix[i][j].w != -101) {
				adjMatrix[i][j].inTree = true;	// Use .inTree to mark the edge already checked
				adjMatrix[j][i].inTree = true;	// Use .inTree to avoid duplicate edges
				edgeRemoved = true;
				unseleceted << i << " " << j << " " << adjMatrix[i][j].w << "\n";
				removedWeight += adjMatrix[i][j].w;
			}
		}
	}

	if (!edgeRemoved) {
		output << 0 << endl;
	} else {
		output << removedWeight << endl;
		output << unseleceted.str() << endl;
		// cout << removedWeight << endl;
	}
}

bool checkColor(vector<Vertex>& vertices, int V) {
	for (int i = 0; i < V; i++) {
		if(vertices[i].color == WHITE)	return false;
	}
	return true;
}


void notInMaxSTDirected(vector<vector<Edge>>& adjMatrix, vector<Edge>& MaxST, ofstream& output, vector<Vertex>& vertices, int V) {
	ostringstream unseleceted;
	int removedWeight = 0;
	bool edgeRemoved = false;

	for(int k = 0; k < 5; k++){
		for(int i = 0; i < V; i++) {
			for(int j = 0; j < V; j++) {
				if(adjMatrix[i][j].w != -101 && adjMatrix[i][j].inTree && adjMatrix[i][j].w < 0) {
					adjMatrix[i][j].inTree = false;
					dfs(adjMatrix, vertices, V);
					bool connected = checkColor(vertices, V);
					if(!connected)	adjMatrix[i][j].inTree = true;
				}
			}
		}


		for(int i = 0; i < V; i++) {
			for(int j = 0; j < V; j++) {
				if(adjMatrix[i][j].w != -101 && !adjMatrix[i][j].inTree && adjMatrix[i][j].w > 0) {
					adjMatrix[i][j].inTree = true;
					dfs(adjMatrix, vertices, V);
					if(cycleFound)	adjMatrix[i][j].inTree = false;
				}
			}
		}
	}
	
	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			if (!adjMatrix[i][j].inTree && i != j && adjMatrix[i][j].w != -101) {
				adjMatrix[i][j].inTree = true;	// Use .inTree to mark the edge already checked
				// adjMatrix[adjMatrix[i][j].v][searchParentIndex(adjMatrix, adjMatrix[i][j].v, i)].inTree = true;	// Use .inTree to avoid duplicate edges
				edgeRemoved = true;
				unseleceted << i << " " << j << " " << adjMatrix[i][j].w << endl;
				removedWeight += adjMatrix[i][j].w;
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

void allPathSetTrue(vector<vector<Edge>>& adjMatrix, int V) {
	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			if(adjMatrix[i][j].w != -101) adjMatrix[i][j].inTree = true;
		}
	}
}

void dfsInit(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int V) {
	for (int i = 0; i < V; i++) {
		vertices[i].key = INT_MIN;		// unused here
		vertices[i].p = -1;
		vertices[i].index = i;
		vertices[i].inTree = false;		// unused here
		vertices[i].color = WHITE;
	}
}

void dfsVisit(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int u, int V) {
	cycleFound = false;
	vertices[u].color = GRAY;
	for (int i = 0; i < V; i++) {
		
		#ifdef DEBUG
		cout << "u = " << u << endl;
		cout << "u color: " << vertices[adjMatrix[u][i].u].color << endl;
		cout << "v = " << adjMatrix[u][i].v << endl;
		cout << "v color: " << vertices[adjMatrix[u][i].v].color << endl;
		cout << "adjMatrix[u][i].inTree: " << (adjMatrix[u][i].inTree ? "True" : "False") << endl;
		cout << endl;
		#endif

		if (vertices[i].color == WHITE && adjMatrix[u][i].inTree) {
			cycleFound = false;
			vertices[i].p = u;
			adjMatrix[u][i].inTree = true;
			if(!cycleFound && vertices[i].color != BLACK) {
				
				#ifdef DEBUG
				cout << "Row 271: " << endl;
				#endif

				dfsVisit(adjMatrix, vertices, i, V);
			} else {
				return;
			}
		} else if (vertices[i].color == GRAY && adjMatrix[u][i].inTree) {
			adjMatrix[u][i].inTree = true;
			vertices[i].p = u;
			
			#ifdef DEBUG
			cout << "vertices[adjMatrix[u][i].v].p: " << vertices[adjMatrix[u][i].v].p << endl;
			cout << "---------------- Cycle Break ----------------" << endl;
			cout << "cycle head: " << adjMatrix[u][i].v << " cycle tail: " << u << endl;
			#endif

			cycleFound = true;
			breakMinEdge(adjMatrix, vertices, i, u);		// breakMinEdge (adjMatrix, cycle head, cycle tail)
			return;
		} else {
			
			#ifdef DEBUG
			cout << "\nPair skipped\n" << endl;
			#endif

			continue;
		}

		if(cycleFound) {
			return;
		}
	}
	vertices[u].color = BLACK;

	#ifdef DEBUG
	cout << endl;
	cout << "u = " << u << " color: " << vertices[u].color << endl;
	cout << "\n\n" << endl;
	#endif

	cycleFound = false;
	return;
}

void dfsVisit(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int u, vector<Edge>& MaxST, int V) {
	vertices[u].color = GRAY;
	for (int i = 0; i < V; i++) {
		if (vertices[i].color == WHITE && adjMatrix[u][i].inTree) {
			cycleFound = false;
			vertices[i].p = u;
			adjMatrix[u][i].inTree = true;
			MaxST.push_back(adjMatrix[u][i]);
			if(!cycleFound && vertices[i].color != BLACK) {
				dfsVisit(adjMatrix, vertices, i, V);
			} else {
				return;
			}
		} else {		// The cycles should have all been broken
			// cout << "\nPair skipped\n" << endl;
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

void dfs(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int V) {
	dfsInit(adjMatrix, vertices, V);
	srand(time(0));
	int root = rand() % V;
	#ifdef DEBUG
	cout << "Root: " << root << endl;
	cout << "Row 330: " << endl;
	#endif

	if(vertices[root].color != BLACK)	dfsVisit(adjMatrix, vertices, root, V);
	if(cycleFound) {
		return;
	} else {
		for (int i = 0; i < V; i++) {
			if (vertices[i].color == WHITE) {

				#ifdef DEBUG
				cout << "DFS Visit: " << i << endl;
				cout << "Row 337: " << endl;
				#endif

				dfsVisit(adjMatrix, vertices, i, V);
				
				#ifdef DEBUG
				cout << "Cycle Found @ Row 337: " << cycleFound << endl;
				#endif

				if(cycleFound) {
					return;
				}
			}
		}
	}

	#ifdef DEBUG
	cout << "Row 348" << endl;
	#endif

	cycleFound = false;
}

void dfs(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int V, vector<Edge>& MaxST){		// This is for build directed MaxST
	
	#ifdef DEBUG
	cout << "Row 400" << endl;
	#endif

	dfsInit(adjMatrix, vertices, V);
	for (int i = 0; i < V; i++) {
		if (vertices[i].color == WHITE) {
			MaxST.push_back(adjMatrix[i][vertices[i].p]);
			dfsVisit(adjMatrix, vertices, i, V);
		}
	}
	// cout << "Last traversal done" << endl;
}

void breakMinEdge(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, int cycleHead, int cycleTail) {
	int u = cycleTail;
	int v = cycleHead;
	int minWeight = adjMatrix[u][v].w;
	Edge minEdge = adjMatrix[u][v];
	
	// cout << "vertices[cycleTail].p: " << vertices[cycleTail].p << endl;

	// if(vertices[cycleHead].p == cycleTail) {
	// 	cout << "SPECIAL CASE" << endl;
	// 	if(adjMatrix[cycleHead][searchParentIndex(adjMatrix, cycleHead, cycleTail)].w < minWeight) {
	// 		minWeight = adjMatrix[cycleHead][searchParentIndex(adjMatrix, cycleHead, cycleTail)].w;
	// 		minEdge = adjMatrix[cycleHead][searchParentIndex(adjMatrix, cycleHead, cycleTail)];
	// 	}
	// 	adjMatrix[cycleTail][searchParentIndex(adjMatrix, cycleTail, cycleHead)].inTree = false;
	// 	cout << "removed Edge: " << cycleTail << " " << cycleHead << " " << adjMatrix[cycleTail][searchParentIndex(adjMatrix, cycleTail, cycleHead)].w << endl;
	// 	cout << "--------------------------------------------------\n" << endl;
	// 	return;
	// }
	
	
	
	do {

		#ifdef DEBUG
		cout << "u = " << u << endl;
		cout << "u color: " << vertices[adjMatrix[u][v].u].color << endl;
		cout << "v = " << adjMatrix[u][v].v << endl;
		cout << "v color: " << vertices[adjMatrix[u][v].v].color << endl;
		#endif

		int temp;
		temp = u;
		u = vertices[u].p;
		v = temp;
		if(adjMatrix[u][v].w < minWeight) {
			minWeight = adjMatrix[u][v].w;
			minEdge = adjMatrix[u][v];
		}
	} while(adjMatrix[u][v].u != cycleHead);
	
	#ifdef DEBUG
	cout << "\nadjMatrix[u][v].u: " << adjMatrix[u][v].u << " cycleHead: " << cycleHead << endl;
	cout << "minWeight: " << minWeight << endl;
	#endif
	
	adjMatrix[minEdge.u][minEdge.v].inTree = false;

	#ifdef DEBUG
	cout << "removed Edge: " << minEdge.u << " " << minEdge.v << " " << minEdge.w << endl;
	cout << "--------------------------------------------------\n" << endl;
	#endif
}

void buildDirectedMaxST(vector<vector<Edge>>& adjMatrix, vector<Vertex>& vertices, vector<Edge>& MaxST, int V) {
	dfs(adjMatrix, vertices, V, MaxST);
}




/* ------------Debug Functions------------- */
void printAdjMatrix(const vector<vector<Edge>>& adjMatrix, int V) {
	for (int i = 0; i < V; ++i) {
		cout << i << ": ";
		for (int j = 0; j < V; ++j) {
			if(adjMatrix[i][j].w != -101) cout << "(" << adjMatrix[i][j].u << ", " << adjMatrix[i][j].v << ", " << adjMatrix[i][j].w;
			#ifdef DEBUG2
			if(adjMatrix[i][j].w != -101) cout << ", " << (adjMatrix[i][j].inTree);
			#endif
			if(adjMatrix[i][j].w != -101) cout << ") ";
		}
		cout << endl;
	}
}

void printMaxST(vector<Edge>& MaxST) {
	cout << "MaxST size: " << MaxST.size() << endl;
	for (const auto& edge : MaxST) {
		cout << "(" << edge.u << ", " << edge.v << ", " << edge.w;
		#ifdef DEBUG2
		if(edge.w != -101) cout << ", " << (edge.inTree);
		#endif
		if(edge.w != -101) cout << ") " << endl;
	}
	cout << endl;
}
