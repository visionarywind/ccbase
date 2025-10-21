/**
 *
给你一个只包含 '(' 和 ')' 的字符串，找出最长有效（格式正确且连续）括号 子串 的长度。

左右括号匹配，即每个左括号都有对应的右括号将其闭合的字符串是格式正确的，比如 "(()())"。



示例 1：

输入：s = "(()"
输出：2
解释：最长有效括号子串是 "()"
示例 2：

输入：s = ")(()())"
输出：6
解释：最长有效括号子串是 "(()())"
示例 3：

输入：s = ""
输出：0


提示：

0 <= s.length <= 3 * 104
s[i] 为 '(' 或 ')'
 *
*/
#include <vector>
#include <stack>
#include <string>
using namespace std;

class Solution {
 public:
  int longestValidParentheses(string s) {
    int ans = 0;
    vector<int> dp(s.length(), 0);
    for (int i = 1; i < s.length(); i++) {
      if (s[i] == ')') {
        if (s[i - 1] == '(') {
          dp[i] = (i > 1 ? dp[i - 2] : 0) + 2;
        } else if (i - 1 - dp[i - 1] >= 0 && s[i - 1 - dp[i - 1]] == '(') {
          dp[i] = dp[i - 1] + (i - 1 - dp[i - 1] - 1 >= 0 ? dp[i - 1 - dp[i - 1] - 1] : 0) + 2;
        }
        ans = max(ans, dp[i]);
      }
    }
    return ans;
  }

  int longestValidParentheses2(string s) {
    int ans;
    stack<int> stk;
    stk.push(-1);
    for (int i = 0; i < s.length(); i++) {
      if (s[i] == '(') {
        stk.push(i);
      } else {
        // here ')' means no '(' in stack
        stk.pop();
        if (stk.empty()) {
          stk.push(i);
        } else {
          ans = max(ans, i - stk.top());
        }
      }
    }
    return ans;
  }
};
