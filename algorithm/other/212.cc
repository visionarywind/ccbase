/**
 *
给定一个 m x n 二维字符网格 board 和一个单词（字符串）列表 words， 返回所有二维网格上的单词 。
单词必须按照字母顺序，通过 相邻的单元格
内的字母构成，其中“相邻”单元格是那些水平相邻或垂直相邻的单元格。同一个单元格内的字母在一个单词中不允许被重复使用。

示例 1：
输入：board = [["o","a","a","n"],["e","t","a","e"],["i","h","k","r"],["i","f","l","v"]], words =
["oath","pea","eat","rain"] 输出：["eat","oath"] 示例 2： 输入：board = [["a","b"],["c","d"]], words = ["abcb"] 输出：[]

提示：
m == board.length
n == board[i].length
1 <= m, n <= 12
board[i][j] 是一个小写英文字母
1 <= words.length <= 3 * 104
1 <= words[i].length <= 10
words[i] 由小写英文字母组成
words 中的所有字符串互不相同
 *
 */
#include <vector>
#include <string>
using namespace std;

class Solution {
 public:
  vector<string> findWords(vector<vector<char>> &board, vector<string> &words) {
    struct Trie {
      Trie *children[26];
      int ref;

      void insert(string &input, int ref) {
        Trie *node = this;
        for (auto c : input) {
          auto idx = c - 'a';
          if (node->children[idx] == nullptr) {
            node->children[idx] = new Trie();
          }
          node = node->children[idx];
        }
        node->ref = ref;
      }
    };

    Trie trie;
    for (int i = 0; i < words.size(); i++) {
      trie.insert(words[i], i);
    }
    vector<string> ans;
    int m = board.size();
    int n = board[0].size();
    auto dfs = [&](this auto &dfs, Trie *node, int i, int j) {
      char c = board[i][j];
      if (node->children[c - 'a'] == nullptr) {
        return;
      }

      node = node->children[c - 'a'];
      board[i][j] = '#';
      if (node->ref != -1) {
        ans.emplace_back(words[node->ref]);
        node->ref = -1;
      }
      int directs[] = {1, 0, -1, 0, 1};
      for (int k = 0; k < 4; k++) {
        int x = i + directs[k], y = j + directs[k + 1];
        if (x >= 0 && x < m && y >= 0 && y < n && board[x][y] != '#') {
          dfs(node, x, y);
        }
      }
      board[i][j] = c;
    };
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        dfs(&trie, i, j);
      }
    }
    return ans;
  }
};
