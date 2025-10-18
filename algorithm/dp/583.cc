#include <string>
#include <vector>
using namespace std;
/**
 * 给定两个单词 s1 和 s2 ，返回使得 s1 和 s2 相同所需的最小步数。每步可以删除任意一个字符串中的一个字符。
 * 函数签名如下：
 * int minDistance(String s1, String s2);
 * 比如输入 s1 = "sea" s2 = "eat"，算法返回 2，第一步将 "sea" 变为 "ea" ，第二步将 "eat" 变为 "ea"。
 */

size_t min_distance(const std::string &word1, const std::string &word2) {
  auto len1 = word1.length();
  auto len2 = word2.length();

  if (len1 == 0) {
    return len2;
  } else if (len2 == 0) {
    return len1;
  }

  vector<vector<size_t>> dp(len1 + 1, vector<size_t>(len2 + 1, 0));
  for (size_t i = 0; i <= len1; i++) {
    dp[i][0] = i;
  }
  for (size_t j = 0; j <= len2; j++) {
    dp[0][j] = j;
  }
  for (size_t i = 0; i < len1; i++) {
    for (size_t j = 0; j < len2; j++) {
      if (word1[i] == word2[j]) {
        dp[i + 1][j + 1] = dp[i][j];
      } else {
        dp[i + 1][j + 1] = std::min(dp[i + 1][j], dp[i][j + 1]) + 1;
      }
    }
  }
  return dp[len1][len2];
}

size_t min_distance_sc(const std::string &word1, const std::string &word2) {
  auto len1 = word1.length();
  auto len2 = word2.length();

  if (len1 == 0) {
    return len2;
  } else if (len2 == 0) {
    return len1;
  }

  vector<size_t> dp(len2 + 1, 0);
  for (size_t i = 0; i <= len2; i++) {
    dp[i] = i;
  }

  for (size_t i = 0; i < len1; i++) {
    size_t prev = dp[0];
    dp[0] = i + 1;
    for (size_t j = 0; j < len2; j++) {
      size_t temp = dp[j + 1];
      if (word1[i] == word2[j]) {
        dp[j + 1] = prev;
      } else {
        dp[j + 1] = std::min({prev, dp[j], dp[j + 1]}) + 1;
      }
      prev = temp;
    }
  }
  return dp[len2];
}
