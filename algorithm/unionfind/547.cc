/**
 *
有 n 个城市，其中一些彼此相连，另一些没有相连。如果城市 a 与城市 b 直接相连，且城市 b 与城市 c 直接相连，那么城市 a
与城市 c 间接相连。

省份 是一组直接或间接相连的城市，组内不含其他没有相连的城市。

给你一个 n x n 的矩阵 isConnected ，其中 isConnected[i][j] = 1 表示第 i 个城市和第 j 个城市直接相连，而
isConnected[i][j] = 0 表示二者不直接相连。

返回矩阵中 省份 的数量。


示例 1：
输入：isConnected = [[1,1,0],[1,1,0],[0,0,1]]
输出：2
示例 2：
输入：isConnected = [[1,0,0],[0,1,0],[0,0,1]]
输出：3


提示：
1 <= n <= 200
n == isConnected.length
n == isConnected[i].length
isConnected[i][j] 为 1 或 0
isConnected[i][i] == 1
isConnected[i][j] == isConnected[j][i]
 *
 */
#include <numeric>
#include <vector>
using namespace std;

class Solution {
 public:
  int findCircleNum(vector<vector<int>> &isConnected) {
    int n = isConnected.size();
    UF uf(n);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (isConnected[i][j] == 1) {
          uf.unite(i, j);
        }
      }
    }
    return uf.cnt;
  }

  struct UF {
    UF(int n) : cnt(n), parent(n) { iota(parent.begin(), parent.end(), 0); }
    void unite(int l, int r) {
      int left = find(l);
      int right = find(r);
      if (left != right) {
        parent[left] = right;
        cnt--;
      }
    }
    int find(int x) {
      if (x != parent[x]) {
        parent[x] = find(parent[x]);
      }
      return parent[x];
    }
    int cnt;
    vector<int> parent;
  };
};