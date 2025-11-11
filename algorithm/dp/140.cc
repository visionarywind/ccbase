/**
 *
给定一个字符串 s 和一个字符串字典 wordDict ，在字符串 s
中增加空格来构建一个句子，使得句子中所有的单词都在词典中。以任意顺序 返回所有这些可能的句子。

注意：词典中的同一个单词可能在分段中被重复使用多次。



示例 1：

输入:s = "catsanddog", wordDict = ["cat","cats","and","sand","dog"]
输出:["cats and dog","cat sand dog"]
示例 2：

输入:s = "pineapplepenapple", wordDict = ["apple","pen","applepen","pine","pineapple"]
输出:["pine apple pen apple","pineapple pen apple","pine applepen apple"]
解释: 注意你可以重复使用字典中的单词。
示例 3：

输入:s = "catsandog", wordDict = ["cats","dog","sand","and","cat"]
输出:[]


提示：

1 <= s.length <= 20
1 <= wordDict.length <= 1000
1 <= wordDict[i].length <= 10
s 和 wordDict[i] 仅有小写英文字母组成
wordDict 中所有字符串都 不同
 *
 */
#include <set>
#include <string>
#include <vector>
using namespace std;

class Solution {
 public:
  vector<string> wordBreak(string s, vector<string> &wordDict) {
    set<string> wordSet{wordDict.begin(), wordDict.end()};
    vector<bool> dp(s.length() + 1, false);
    dp[0] = true;
    for (int i = 1; i < s.length(); i++) {
      for (int j = i - 1; j >= 0; j--) {
        if (wordSet.count(s.substr(j, i - j)) != 0 && dp[j]) {
          dp[i] = true;
          break;
        }
      }
    }
    vector<string> ans;
    dfs(s, s.length(), dp, wordSet, "", ans);
    return ans;
  }

  void dfs(string &s, int index, vector<bool> &dp, set<string> &wordSet, string temp, vector<string> &ans) {
    if (index == 0) {
      ans.emplace_back(temp);
      return;
    }
    for (int i = index - 1; i >= 0; i--) {
      string str = s.substr(i, index - i);
      if (dp[i] && wordSet.count(str) != 0) {
        dfs(s, i, dp, wordSet, temp + str, ans);
      }
    }
  }
};
