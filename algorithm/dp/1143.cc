#include <string>
#include <vector>

size_t lcs(const std::string &left, const std::string &right) {
  auto left_len = left.length();
  if (left_len == 0) {
    return 0;
  }
  auto right_len = right.length();
  if (right_len == 0) {
    return 0;
  }

  std::vector<std::vector<size_t>> dp(left_len + 1, std::vector<size_t>(right_len + 1, 0));
  for (size_t i = 0; i < left_len; i++) {
    for (size_t j = 0; j < right_len; j++) {
      if (left[i] == right[j]) {
        dp[i + 1][j + 1] = dp[i][j] + 1;
      } else {
        dp[i + 1][j + 1] = std::max(dp[i + 1][j], dp[i][j + 1]);
      }
    }
  }

  return dp[left_len][right_len];
}

/**
 * prev means result of left corner, use temp to store recent dp value.
 */
size_t lcs_sc(const std::string &left, const std::string &right) {
  auto left_len = left.length();
  if (left_len == 0) {
    return 0;
  }
  auto right_len = right.length();
  if (right_len == 0) {
    return 0;
  }

  std::vector<size_t> dp(right_len + 1, 0);
  for (size_t i = 0; i < left_len; i++) {
    size_t prev = 0;
    for (size_t j = 0; j < right_len; j++) {
      size_t temp = dp[j + 1];
      if (left[i] == right[j]) {
        dp[j + 1] = prev + 1;
      } else {
        dp[j + 1] = std::max(prev, dp[j + 1]);
      }
      prev = temp;
    }
  }

  return dp[right_len];
}