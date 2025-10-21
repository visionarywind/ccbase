/**
 *
  给你一个输入字符串 (s) 和一个字符模式 (p) ，请你实现一个支持 '?' 和 '*' 匹配规则的通配符匹配：
  '?' 可以匹配任何单个字符。
  '*' 可以匹配任意字符序列（包括空字符序列）。
  判定匹配成功的充要条件是：字符模式必须能够 完全匹配 输入字符串（而不是部分匹配）。


  示例 1：
    输入：s = "aa", p = "a"
    输出：false
    解释："a" 无法匹配 "aa" 整个字符串。

  示例 2：
    输入：s = "aa", p = "*"
    输出：true
    解释：'*' 可以匹配任意字符串。

  示例 3：
    输入：s = "cb", p = "?a"
    输出：false
    解释：'?' 可以匹配 'c', 但第二个 'a' 无法匹配 'b'。


  提示：
    0 <= s.length, p.length <= 2000
    s 仅由小写英文字母组成
    p 仅由小写英文字母、'?' 或 '*' 组成
 *
*/
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class Solution {
 public:
  bool isMatch(string s, string p) {
    int slen = s.length();
    int plen = p.length();
    vector<vector<bool>> dp(slen + 1, vector<bool>(plen + 1, false));
    dp[0][0] = true;
    for (int i = 0; i < plen && p[i] == '*'; i++) {
      dp[0][i + 1] = true;
    }
    for (int i = 0; i < slen; i++) {
      for (int j = 0; j < plen; j++) {
        if (s[i] == p[j] || p[j] == '?') {
          dp[i + 1][j + 1] = dp[i][j];
        } else if (p[j] == '*') {
          dp[i + 1][j + 1] = dp[i][j + 1] | dp[i + 1][j];
        }
      }
    }
    return dp[slen][plen];
  }

  bool isMatch2(string s, string p) {
    int j = 0;
    int start = 0;
    int candidate = 0;

    for (int i = 0; i < s.length();) {
      if (j < p.length() && (s[i] == p[j] || p[j] == '?')) {
        i++;
        j++;
      } else if (j < p.length() && p[j] == '*') {
        start = ++j;
        candidate = i;
      } else if (start != 0) {
        i = ++candidate;
        j = start;
      } else {
        return false;
      }
    }
    while (j < p.length() && p[j] == '*') {
      j++;
    }
    return j == p.length();
  }
};

int main() {
  Solution s;
  cout << s.isMatch("", "****") << endl;
  cout << s.isMatch("aa", "*") << endl;
  cout << s.isMatch("abcd", "a*e") << endl;
  cout << s.isMatch("aaavcd", "a*v*d") << endl;
  cout << s.isMatch("ae", "a*e") << endl;
  cout << s.isMatch(
            "bbbbaaaaabaabbbbaabaaabaabbababbbaaabbababbbabaabaabaabababaaabaaaabbaabbaabbaaaaabbabbbbaaaababbaaaabbabb"
            "baabaaabbaabaabaaababbabbaababaababbbbbaabbabbabbbbaabbaaababbabaaabbbbbbbbaababbbbbbabbaabaaa",
            "b*a**b***abaabaaaba*abaaaaabaabb*bbb*aa*ab*a**b**b*a**a**a*abbb***bb*b*****baababaa**ab*aa*bbaba**bb*b*")
       << endl;
  cout << s.isMatch2("", "****") << endl;
  cout << s.isMatch2("aa", "*") << endl;
  cout << s.isMatch2("abcd", "a*e") << endl;
  cout << s.isMatch2("aaavcd", "a*v*d") << endl;
  cout << s.isMatch2("ae", "a*e") << endl;
  cout << s.isMatch2(
            "bbbbaaaaabaabbbbaabaaabaabbababbbaaabbababbbabaabaabaabababaaabaaaabbaabbaabbaaaaabbabbbbaaaababbaaaabbabb"
            "baabaaabbaabaabaaababbabbaababaababbbbbaabbabbabbbbaabbaaababbabaaabbbbbbbbaababbbbbbabbaabaaa",
            "b*a**b***abaabaaaba*abaaaaabaabb*bbb*aa*ab*a**b**b*a**a**a*abbb***bb*b*****baababaa**ab*aa*bbaba**bb*b*")
       << endl;
  return 0;
}