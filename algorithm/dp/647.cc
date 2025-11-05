/**
 *
给你一个字符串 s ，请你统计并返回这个字符串中 回文子串 的数目。

回文字符串 是正着读和倒过来读一样的字符串。

子字符串 是字符串中的由连续字符组成的一个序列。



示例 1：

输入：s = "abc"
输出：3
解释：三个回文子串: "a", "b", "c"
示例 2：

输入：s = "aaa"
输出：6
解释：6个回文子串: "a", "a", "a", "aa", "aa", "aaa"


提示：

1 <= s.length <= 1000
s 由小写英文字母组成
 *
*/
#include <string>
#include <vector>
using namespace std;

class Solution {
 public:
  int countSubstrings(string s) {
    int len = s.length();
    if (len == 1) {
      return 1;
    }
    vector<vector<bool>> dp(len, vector<bool>(len, false));  // 子串i-j是否为回文子串
    int ans;
    for (int i = len - 1; i >= 0; i--) {
      for (int j = i; j < len; j++) {
        if (s[i] == s[j]) {
          dp[i][j] = j - i < 2 ? true : dp[i + 1][j - 1];
          if (dp[i][j]) {
            ans++;
          }
        }
      }
    }
    return ans;
  }
};