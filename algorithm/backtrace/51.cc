/**
 *
输入：n = 4
输出：[[".Q..","...Q","Q...","..Q."],["..Q.","Q...","...Q",".Q.."]]
解释：如上图所示，4 皇后问题存在两个不同的解法。
示例 2：

输入：n = 1
输出：[["Q"]]
 *
*/
#include <vector>
#include <string>
using namespace std;

class Solution {
 public:
  vector<vector<string>> nSolveQueue(int n) {
    vector<vector<string>> ans;
    vector<vector<char>> chess(n, vector<char>(n, '.'));
    backtrace(&ans, &chess, n, 0);
    return ans;
  }

  void backtrace(vector<vector<string>> *ans, vector<vector<char>> *chess, int n, int row) {
    if (row == n) {
      vector<string> res;
      for (auto &vec : *chess) {
        string s;
        for (auto c : vec) {
          s += c;
        }
        res.emplace_back(s);
      }
      ans->emplace_back(res);
      return;
    }

    for (int col = 0; col < n; col++) {
      if (!valid(chess, n, row, col)) {
        continue;
      }
      (*chess)[row][col] = 'Q';
      backtrace(ans, chess, n, row + 1);
      (*chess)[row][col] = '.';
    }
  }

  bool valid(vector<vector<char>> *chess, int n, int row, int col) {
    for (int i = 0; i < n; i++) {
      if ((*chess)[i][col] == 'Q' || (*chess)[row][i] == 'Q') {
        return false;
      }
    }
    for (int i = 0; row - i >= 0 && col - i >= 0; i++) {
      if ((*chess)[row - i][col - i] == 'Q') {
        return false;
      }
    }
    for (int i = 0; row - i >= 0 && row - i < n && col + i < n; i++) {
      if ((*chess)[row - i][col + i] == 'Q') {
        return false;
      }
    }
    return true;
  }
};