/**
 *
给定一个由 n 个节点组成的网络，用 n x n 个邻接矩阵 graph 表示。在节点网络中，只有当 graph[i][j] = 1 时，节点 i
能够直接连接到另一个节点 j。 一些节点 initial
最初被恶意软件感染。只要两个节点直接连接，且其中至少一个节点受到恶意软件的感染，那么两个节点都将被恶意软件感染。这种恶意软件的传播将继续，直到没有更多的节点可以被这种方式感染。
假设 M(initial) 是在恶意软件停止传播之后，整个网络中感染恶意软件的最终节点数。
我们可以从 initial 中 删除一个节点，并完全移除该节点以及从该节点到任何其他节点的任何连接。

请返回移除后能够使 M(initial) 最小化的节点。如果有多个节点满足条件，返回索引 最小的节点 。


示例 1：
输入：graph = [[1,1,0],[1,1,0],[0,0,1]], initial = [0,1]
输出：0
示例 2：
输入：graph = [[1,1,0],[1,1,1],[0,1,1]], initial = [0,1]
输出：1
示例 3：
输入：graph = [[1,1,0,0],[1,1,1,0],[0,1,1,1],[0,0,1,1]], initial = [0,1]
输出：1

提示：
n == graph.length
n == graph[i].length
2 <= n <= 300
graph[i][j] 是 0 或 1.
graph[i][j] == graph[j][i]
graph[i][i] == 1
1 <= initial.length < n
0 <= initial[i] <= n - 1
 initial 中每个整数都不同
 *
 */
#include <set>
#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
 public:
  int minMalwareSpread(vector<vector<int>> &graph, vector<int> &initial) {
    set<int> initialSet(initial.begin(), initial.end());
    int n = graph.size();
    vector<int> visited(n, false);

    auto dfs = [&](this auto &&dfs, int i) -> pair<int, int> {
      visited[i] = true;
      int cnt = 1;
      int node_id = -1;
      for (int j = 0; j < n; j++) {
        if (graph[i][j] == 0) {
          continue;
        }
        if (initialSet.count(j) > 0) {
          if (node_id == -1) {
            node_id = j;
          } else if (node_id != j) {
            node_id = -2;
          }
        } else if (!visited[j]) {
          auto [sub_node_id, sub_node_cnt] = dfs(j);
          if (sub_node_id == -2 || node_id == -1) {
            node_id = sub_node_id;
          } else if (node_id != -2 && sub_node_id > 0 && node_id != sub_node_id) {
            node_id = -2;
          }
          cnt += sub_node_cnt;
        }
      }
      return {node_id, cnt};
    };

    unordered_map<int, int> cnts;
    for (int i = 0; i < n; i++) {
      if (initialSet.count(i) != 0 || visited[i]) {
        continue;
      }
      auto [init_id, node_cnt] = dfs(i);
      if (init_id >= 0) {
        cnts[init_id] += node_cnt;
      }
    }

    int max_cnt = 0;
    int node_id = -1;
    for (auto [id, cnt] : cnts) {
      if (cnt > max_cnt || (cnt == max_cnt && id < node_id)) {
        max_cnt = cnt;
        node_id = id;
      }
    }

    return node_id == -1 ? *min_element(initial.begin(), initial.end()) : node_id;
  }
};
