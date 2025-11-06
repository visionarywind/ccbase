/**
 *
给定一个 m x n 整数矩阵 matrix ，找出其中 最长递增路径 的长度。
对于每个单元格，你可以往上，下，左，右四个方向移动。 不能 在 对角线 方向上移动或移动到 边界外（即不允许环绕）。

示例 1：
输入：matrix = [[9,9,4],[6,6,8],[2,1,1]]
输出：4
解释：最长递增路径为 [1, 2, 6, 9]。
示例 2：
输入：matrix = [[3,4,5],[3,2,6],[2,2,1]]
输出：4
解释：最长递增路径是 [3, 4, 5, 6]。注意不允许在对角线方向上移动。
示例 3：
输入：matrix = [[1]]
输出：1

提示：
m == matrix.length
n == matrix[i].length
1 <= m, n <= 200
0 <= matrix[i][j] <= 231 - 1
 *
 */
#include <vector>
using namespace std;

class Solution {
 public:
  int longestIncreasingPath(vector<vector<int>> &matrix) {
    int ans = 0;
    int row = matrix.size();
    int col = matrix[0].size();
    vector<vector<int>> memo(row, vector<int>(col, 0));
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
        ans = max(ans, dfs(matrix, memo, i, j));
      }
    }
    return ans;
  }

  int dfs(vector<vector<int>> &matrix, vector<vector<int>> &memo, int i, int j) {
    if (memo[i][j] != 0) {
      return memo[i][j];
    }
    memo[i][j] = 1;
    static vector<vector<int>> directories = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (auto &vec : directories) {
      int row = i + vec[0];
      int col = j + vec[1];
      if (row >= 0 && row < matrix.size() && col >= 0 && col < matrix[0].size() && matrix[row][col] > matrix[i][j]) {
        memo[i][j] = max(memo[i][j], dfs(matrix, memo, row, col) + 1);
      }
    }
    return memo[i][j];
  }
};