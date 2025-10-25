/**
 *
给定一个字符串 s ，请你找出其中不含有重复字符的 最长 子串 的长度。



示例 1:

输入: s = "abcabcbb"
输出: 3
解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。注意 "bca" 和 "cab" 也是正确答案。
示例 2:

输入: s = "bbbbb"
输出: 1
解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。
示例 3:

输入: s = "pwwkew"
输出: 3
解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
     请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。


提示：

0 <= s.length <= 5 * 104
s 由英文字母、数字、符号和空格组成
 *
*/
#include <unordered_set>
#include <string>
using namespace std;

class Solution {
 public:
  int lengthOfLongestSubstring(string s) {
    unordered_set<char> cnt;
    int right = 0;
    int ans = 0;
    for (int i = 0; i < s.length(); i++) {
      if (i != 0) {
        cnt.erase(s[i - 1]);
      }
      while (right < s.length() && cnt.count(s[right]) == 0) {
        cnt.insert(s[right++]);
      }
      ans = max(ans, right - i);
    }
    return ans;
  }

  int lengthOfLongestSubstringLowPerf(string s) {
    unordered_set<char> cnt;
    int left = 0;
    int ans = 0;
    for (int i = 0; i < s.length();) {
      while (cnt.count(s[i]) == 0 && i < s.length()) {
        cnt.insert(s[i]);
        i++;
      }
      ans = max(ans, i - left);
      if (i >= s.length()) {
        break;
      }
      while (cnt.count(s[i]) != 0) {
        cnt.erase(s[left++]);
      }
      cnt.insert(s[i]);
      i++;
    }
    return ans;
  }
};