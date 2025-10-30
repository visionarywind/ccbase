/**
 *
给你一个字符串 s，请你将 s 分割成一些 子串，使每个子串都是 回文串 。返回 s 所有可能的分割方案。



示例 1：

输入：s = "aab"
输出：[["a","a","b"],["aa","b"]]
示例 2：

输入：s = "a"
输出：[["a"]]


提示：

1 <= s.length <= 16
s 仅由小写英文字母组成
 *
*/
#include <string>
#include <vector>
using namespace std;

class Solution {
 public:
  vector<vector<string>> partition(string s) {
    vector<vector<string>> ans;
    vector<string> path;
    dfs(s, 0, s.length() - 1, &ans, &path);
    return ans;
  }

  void dfs(string s, int start, int end, vector<vector<string>> *ans, vector<string> *path) {
    if (start > end) {
      ans->emplace_back(*path);
      return;
    }
    for (int i = start; i <= end; i++) {
      if (check(s, start, i)) {
        path->emplace_back(s.substr(start, i - start + 1));
        dfs(s, i + 1, end, ans, path);
        path->pop_back();
      }
    }
  }

  bool check(string &s, int left, int right) {
    while (left < right) {
      if (s[left] != s[right]) {
        return false;
      }
      left++;
      right--;
    }
    return true;
  }

  vector<vector<string>> partitionDp(string s) {
    vector<vector<string>> ans;
    vector<string> path;
    int len = s.length();
    vector<vector<bool>> dp(len, vector<bool>(len, false));
    for (int i = 0; i < len; i++) {
      for (int j = 0; j <= i; j++) {
        if (s[j] == s[i] && (i - j < 2 || dp[j + 1][i - 1])) {
          dp[j][i] = true;
        }
      }
    }
    dfsDp(s, 0, len - 1, dp, &ans, &path);
    return ans;
  }

  void dfsDp(string &s, int start, int end, vector<vector<bool>> &dp, vector<vector<string>> *ans,
             vector<string> *path) {
    if (start > end) {
      ans->emplace_back(*path);
      return;
    }
    for (int i = start; i <= end; i++) {
      if (dp[start][i]) {
        path->emplace_back(s.substr(start, i - start + 1));
        dfsDp(s, i + 1, end, dp, ans, path);
        path->pop_back();
      }
    }
  }
};