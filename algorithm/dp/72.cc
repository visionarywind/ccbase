/**
 *
给你两个单词 word1 和 word2， 请返回将 word1 转换成 word2 所使用的最少操作数  。

你可以对一个单词进行如下三种操作：

插入一个字符
删除一个字符
替换一个字符


示例 1：
输入：word1 = "horse", word2 = "ros"
输出：3
解释：
horse -> rorse (将 'h' 替换为 'r')
rorse -> rose (删除 'r')
rose -> ros (删除 'e')

示例 2：
输入：word1 = "intention", word2 = "execution"
输出：5
解释：
intention -> inention (删除 't')
inention -> enention (将 'i' 替换为 'e')
enention -> exention (将 'n' 替换为 'x')
exention -> exection (将 'n' 替换为 'c')
exection -> execution (插入 'u')


提示：

0 <= word1.length, word2.length <= 500
word1 和 word2 由小写英文字母组成
 *
*/
#include <string>
#include <vector>
using namespace std;

class Solution {
 public:
  int minDistance(string word1, string word2) {
    int len1 = word1.length();
    int len2 = word2.length();
    vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1, 0));
    for (int i = 0; i < len1; i++) {
      dp[i + 1][0] = i + 1;
    }
    for (int i = 0; i < len2; i++) {
      dp[0][i + 1] = i + 1;
    }
    for (int i = 0; i < len1; i++) {
      for (int j = 0; j < len2; j++) {
        if (word1[i] == word2[j]) {
          dp[i + 1][j + 1] = dp[i][j];
        } else {
          dp[i + 1][j + 1] = 1 + min(dp[i][j], min(dp[i][j + 1], dp[i + 1][j]));
        }
      }
    }
    return dp[len1][len2];
  }

  int minDistanceDfs(string word1, string word2) {
    int len1 = word1.length();
    int len2 = word2.length();
    vector<vector<int>> memo(len1 + 1, vector<int>(len2 + 1, 0));
    return dfs(word1, word2, len1, len2, &memo);
  }

  int dfs(string word1, string word2, int left, int right, vector<vector<int>> *memo) {
    if (left == 0 || right == 0) {
      (*memo)[left][right] = left + right;
      return (*memo)[left][right];
    }

    if ((*memo)[left][right] != 0) {
      return (*memo)[left][right];
    }

    if (word1[left - 1] == word2[right - 1]) {
      (*memo)[left][right] = dfs(word1, word2, left - 1, right - 1, memo);
    } else {
      (*memo)[left][right] =
        1 + min(min(dfs(word1, word2, left, right - 1, memo), dfs(word1, word2, left - 1, right, memo)),
                dfs(word1, word2, left - 1, right - 1, memo));
    }
    return (*memo)[left][right];
  }
};
