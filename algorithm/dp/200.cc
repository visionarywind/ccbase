/**
 *
给你一个由 '1'（陆地）和 '0'（水）组成的的二维网格，请你计算网格中岛屿的数量。

岛屿总是被水包围，并且每座岛屿只能由水平方向和/或竖直方向上相邻的陆地连接形成。

此外，你可以假设该网格的四条边均被水包围。



示例 1：

输入：grid = [
  ['1','1','1','1','0'],
  ['1','1','0','1','0'],
  ['1','1','0','0','0'],
  ['0','0','0','0','0']
]
输出：1
示例 2：

输入：grid = [
  ['1','1','0','0','0'],
  ['1','1','0','0','0'],
  ['0','0','1','0','0'],
  ['0','0','0','1','1']
]
输出：3


提示：

m == grid.length
n == grid[i].length
1 <= m, n <= 300
grid[i][j] 的值为 '0' 或 '1'
 *
 */
#include <vector>
using namespace std;

class SolutionBacktrace {
 public:
  int numIslands(vector<vector<char>> &grid) {
    int ans = 0;
    int m = grid.size();
    int n = grid[0].size();
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (grid[i][j] == '1') {
          ans++;
          dfs(grid, i, j);
        }
      }
    }
    return ans;
  }

  void dfs(vector<vector<char>> &grid, int left, int right) {
    static vector<vector<int>> directories{{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
    grid[left][right] = '0';
    for (auto directory : directories) {
      auto i = left + directory[0];
      auto j = right + directory[1];
      if (i < 0 || j < 0 || i >= grid.size() || j >= grid[0].size()) {
        continue;
      }
      if (grid[i][j] == '1') {
        dfs(grid, i, j);
      }
    }
  }
};

class Solution {
 public:
  int numIslands(vector<vector<char>> &grid) {
    UF uf{grid};
    for (int i = 0, endI = grid.size(); i < endI; i++) {
      for (int j = 0, endJ = grid[i].size(); j < endJ; j++) {
        if (grid[i][j] == '1') {
          grid[i][j] = '0';
          auto index = i * endJ + j;
          if (i - 1 >= 0 && grid[i - 1][j] == '1') {
            uf.connect(index, (i - 1) * endJ + j);
          }
          if (i + 1 < endI && grid[i + 1][j] == '1') {
            uf.connect(index, (i + 1) * endJ + j);
          }
          if (j - 1 >= 0 && grid[i][j - 1] == '1') {
            uf.connect(index, i * endJ + j - 1);
          }
          if (j + 1 < endJ && grid[i][j + 1] == '1') {
            uf.connect(index, i * endJ + j + 1);
          }
        }
      }
    }
    return uf.count();
  }

  struct UF {
    int count_{0};
    vector<int> parent_;

    UF(vector<vector<char>> &grid) {
      for (int i = 0, endI = grid.size(); i < endI; i++) {
        for (int j = 0, endJ = grid[i].size(); j < endJ; j++) {
          auto index = i * endJ + j;
          parent_.push_back(index);
          if (grid[i][j] == '1') {
            count_++;
          }
        }
      }
    }

    void connect(int from, int to) {
      int fromParent = find(from);
      int toParent = find(to);
      if (fromParent != toParent) {
        parent_[fromParent] = toParent;
        count_--;
      }
    }

    bool connected(int from, int to) { return parent_[from] == parent_[to]; }

    int find(int x) {
      if (parent_[x] != x) {
        parent_[x] = find(parent_[x]);
      }
      return parent_[x];
    }

    int count() { return count_; }
  };
};
