/**
 *
给定一个仅包含 0 和 1 、大小为 rows x cols 的二维二进制矩阵，找出只包含 1 的最大矩形，并返回其面积。



示例 1：


输入：matrix = [["1","0","1","0","0"],["1","0","1","1","1"],["1","1","1","1","1"],["1","0","0","1","0"]]
输出：6
解释：最大矩形如上图所示。
示例 2：

输入：matrix = [["0"]]
输出：0
示例 3：

输入：matrix = [["1"]]
输出：1


提示：

rows == matrix.length
cols == matrix[0].length
1 <= rows, cols <= 200
matrix[i][j] 为 '0' 或 '1'
 *
*/
#include <deque>
#include <vector>
using namespace std;

class Solution {
 public:
  int maximalRectangle(vector<vector<char>> &matrix) {
    vector<int> heights(matrix[0].size(), 0);
    int ans = 0;
    for (int i = 0; i < matrix.size(); i++) {
      for (int j = 0; j < matrix[i].size(); j++) {
        if (matrix[i][j] == '1') {
          heights[j]++;
        } else {
          heights[j] = 0;
        }
      }
      ans = max(ans, maximalRectangle(heights));
    }
    return ans;
  }

  int maximalRectangle(vector<int> heights) {
    heights.insert(heights.begin(), 0);
    heights.push_back(0);
    deque<int> q;
    int ans = 0;
    for (int i = 0; i < heights.size(); i++) {
      while (!q.empty() && heights[i] < heights[q.back()]) {
        int idx = q.back();
        q.pop_back();
        int width = i - 1 - (q.back() + 1) + 1;
        ans = max(ans, heights[idx] * width);
      }
      q.push_back(i);
    }
    return ans;
  }
};