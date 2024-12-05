#include <thread>
#include <iostream>
#include <vector>

using namespace std;

struct Node {
  Node() { cout << "Node construct" << endl; }

  ~Node() {
    cout << "Node deconstruct" << endl;
    for (auto v : vec_) cout << "v : " << v << std::endl;
  }

  void Push(int i) {
    cout << "push : " << i << endl;
    vec_.emplace_back(i);
  }
  vector<int> vec_;
};

struct Profiler {
  void test() {
    thread_local Node kNode;
    for (int i = 0; i < 10; i++) {
      std::thread t([&]() {
        cout << "start thread : " << i << endl;
        kNode.Push(i);
        cout << "end thread : " << i << endl;
      });
      t.join();
    }
  }
};

int main() {
  Profiler p;
  p.test();
  return 0;
}