#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct Edge {
    int from, to, weight;
    bool operator<(const Edge& other) const {
        if (from != other.from) return from < other.from;
        return to < other.to;
    }
};

vector<int> parent;

int find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void unite(int x, int y) {
    parent[find(x)] = find(y);
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    if (argc != 5) {
        cerr << "使用方式: " << argv[0] << " <頂點數量> <邊數量> <圖形類型:u/d> <輸出檔名>" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    char graph_type = argv[3][0];
    
    ofstream fout(argv[4], ios::out | ios::binary);
    fout.rdbuf()->pubsetbuf(nullptr, 0);  // 關閉緩衝

    if (graph_type == 'u' && (n > 10000 || m > 20000000)) {
        cerr << "Invalid input for undirected graph!" << endl;
        return 1;
    }
    if (graph_type == 'd' && (n > 5000 || m > 10000)) {
        cerr << "Invalid input for directed graph!" << endl;
        return 1;
    }
    
    if (m < n-1) {
        cerr << "Not enough edges to ensure connectivity!" << endl;
        return 1;
    }

    // 預分配記憶體
    parent.reserve(n);
    vector<Edge> edge_vector;
    edge_vector.reserve(m);
    
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_int_distribution<> vertex_dist(0, n-1);
    uniform_int_distribution<> weight_dist(-100, 100);

    set<Edge> edges;
    
    // Initialize union-find
    parent.resize(n);
    for(int i = 0; i < n; i++) {
        parent[i] = i;
    }
    
    // 使用stringstream進行快取輸出
    stringstream ss;
    ss << graph_type << '\n' << n << '\n' << m << '\n';
    
    // First generate a spanning tree
    for(int i = 0; i < n-1; ) {
        int from = vertex_dist(gen);
        int to = vertex_dist(gen);
        
        if(from == to) continue;
        if(find(from) == find(to)) continue;
        
        Edge e{from, to, weight_dist(gen)};
        if(graph_type == 'u' && from > to) {
            swap(e.from, e.to);
        }
        
        if(edges.insert(e).second) {
            unite(from, to);
            edge_vector.push_back(e);
            i++;
        }
    }
    
    // Then add remaining random edges
    while(edges.size() < m) {
        int from = vertex_dist(gen);
        int to = vertex_dist(gen);
        
        if(from == to) continue;
        
        Edge e{from, to, weight_dist(gen)};
        if(graph_type == 'u' && from > to) {
            swap(e.from, e.to);
        }
        
        if(edges.insert(e).second) {
            edge_vector.push_back(e);
        }
    }

    // 批次寫入
    for(const Edge& e : edge_vector) {
        ss << e.from << ' ' << e.to << ' ' << e.weight << '\n';
    }
    ss << "0\n";
    
    fout << ss.str();
    fout.close();

    return 0;
}