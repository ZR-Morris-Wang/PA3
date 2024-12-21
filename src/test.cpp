#include <iostream>
#include <queue>

using namespace std;

int main() {
    priority_queue<int, vector<int>, greater<int>> minHeap;

    for (int i = 0; i < 10; ++i) {
        minHeap.push(rand() % 100);
    }
    
    while(!minHeap.empty()) {
        cout << minHeap.top() << " ";
        minHeap.pop();
    }
    cout << endl;

    return 0;
}