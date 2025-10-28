/**
 *
编写一个程序，通过填充空格来解决数独问题。

数独的解法需 遵循如下规则：

数字 1-9 在每一行只能出现一次。
数字 1-9 在每一列只能出现一次。
数字 1-9 在每一个以粗实线分隔的 3x3 宫内只能出现一次。（请参考示例图）
数独部分空格内已填入了数字，空白格用 '.' 表示。



示例 1：


输入：board =
[["5","3",".",".","7",".",".",".","."],["6",".",".","1","9","5",".",".","."],[".","9","8",".",".",".",".","6","."],["8",".",".",".","6",".",".",".","3"],["4",".",".","8",".","3",".",".","1"],["7",".",".",".","2",".",".",".","6"],[".","6",".",".",".",".","2","8","."],[".",".",".","4","1","9",".",".","5"],[".",".",".",".","8",".",".","7","9"]]
输出：[["5","3","4","6","7","8","9","1","2"],["6","7","2","1","9","5","3","4","8"],["1","9","8","3","4","2","5","6","7"],["8","5","9","7","6","1","4","2","3"],["4","2","6","8","5","3","7","9","1"],["7","1","3","9","2","4","8","5","6"],["9","6","1","5","3","7","2","8","4"],["2","8","7","4","1","9","6","3","5"],["3","4","5","2","8","6","1","7","9"]]
解释：输入的数独如上图所示，唯一有效的解决方案如下所示：




提示：

board.length == 9
board[i].length == 9
board[i][j] 是一位数字或者 '.'
题目数据 保证 输入数独仅有一个解
 *
*/
#include <vector>
using namespace std;

class Solution {
 public:
  void solveSudoku(vector<vector<char>> &board) {
    vector<pair<int, int>> candidates;
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        if (board[i][j] == '.') {
          candidates.emplace_back(make_pair(i, j));
        } else {
          int val = board[i][j] - '0' - 1;
          row[i][val] = col[val][j] = block[i / 3][j / 3][val] = true;
        }
      }
    }
    dfs(board, candidates, 0);
  }

  void dfs(vector<vector<char>> &board, vector<pair<int, int>> &candidates, int pos) {
    if (pos == candidates.size() || success_) {
      success_ = true;
      return;
    }
    auto [i, j] = candidates[pos];
    for (int k = 0; k < 9; k++) {
      if (row[i][k] || col[k][j] || block[i / 3][j / 3][k]) {
        continue;
      }
      row[i][k] = col[k][j] = block[i / 3][j / 3][k] = true;
      board[i][j] = k + '0' + 1;
      dfs(board, candidates, pos + 1);
      row[i][k] = col[k][j] = block[i / 3][j / 3][k] = false;
    }
  }

 private:
  bool success_{false};
  bool row[9][9]{false};
  bool col[9][9]{false};
  bool block[3][3][9]{false};
};