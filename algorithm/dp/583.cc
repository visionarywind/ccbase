#include <string>
#include <vector>
/**
 * 给定两个单词 s1 和 s2 ，返回使得 s1 和 s2 相同所需的最小步数。每步可以删除任意一个字符串中的一个字符。
 * 函数签名如下：
 * int minDistance(String s1, String s2);
 * 比如输入 s1 = "sea" s2 = "eat"，算法返回 2，第一步将 "sea" 变为 "ea" ，第二步将 "eat" 变为 "ea"。
 */

size_t min_distance(const std::string &left, const std::string &right) {
  auto llen = left.length();
  auto rlen = right.length();

  if (llen == 0) {
    return rlen;
  } else if (rlen == 0) {
    return llen;
  }

  std::vector<std::vector<size_t>> dp(llen + 1, std::vector<size_t>(rlen + 1, 0));
  for (size_t i = 0; i < llen; i++) {
    dp[i][0] = i;
  }
  for (size_t j = 0; j < rlen; j++) {
    dp[0][j] = j;
  }
  for (size_t i = 0; i < llen; i++) {
    for (size_t j = 0; j < rlen; j++) {
      if (left[i] == right[j]) {
        dp[i + 1][j + 1] = dp[i][j];
      } else {
        dp[i + 1][j + 1] = std::min(dp[i + 1, j], dp[i, j + 1]) - 1;
      }
    }
  }
  return dp[llen][rlen];
}

size_t min_distance_sc(const std::string &left, const std::string &right) {
  auto llen = left.length();
  auto rlen = right.length();

  if (llen == 0) {
    return rlen;
  } else if (rlen == 0) {
    return llen;
  }

  std::vector<size_t> dp(rlen + 1, 0);
  for (size_t j = 0; j < rlen; j++) {
    dp[j] = j;
  }
  for (size_t i = 0; i < llen; i++) {
    size_t prev = 0;
    for (size_t j = 0; j < rlen; j++) {
      size_t temp = dp[j + 1];
      if (left[i] == right[j]) {
        dp[j + 1] = prev;
      } else {
        dp[j + 1] = std::min(dp[j], dp[j + 1]) + 1;
      }
      prev = temp;
    }
  }
  return dp[rlen];
}
