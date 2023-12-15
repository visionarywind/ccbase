#include <iostream>
#include <queue>
#include <vector>
using namespace std;

void PriorityQueueTest() {
  auto cmp = [](int l, int r) { return l > r; };
  priority_queue<int, std::vector<int>, decltype(cmp)> pq(cmp);
  vector<int> vec{2, 4, 5, 1, 9, 0, -9, -1};
  for (int v : vec) {
    pq.emplace(v);
  }
  // pq has no iterator
  // auto &&it = std::lower_bound(pq.begin(), pq.end(), 3);
  // cout << *it << endl;
  while (!pq.empty()) {
    cout << pq.top() << " ";
    pq.pop();
  }
  cout << endl;
}