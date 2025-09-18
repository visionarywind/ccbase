#include <cctype>
#include <string_view>
#include <vector>

std::vector<int> BuildPatternTable(const std::string_view &pattern) {
  std::vector<int> pattern_table(pattern.size(), 0);
  for (int i = 1, j = 0; i < pattern.size(); i++) {
    while (j > 0 && pattern[i] != pattern[j]) {
      j = pattern_table[j - 1];
    }
    if (pattern[i] == pattern[j]) {
      j++;
    }
    pattern_table[i] = j;
  }
  return pattern_table;
}

bool kmp(const std::string_view &src, const std::string_view &pattern) {
  if (pattern.empty()) {
    return true;
  }
  if (src.empty()) {
    return false;
  }
  const auto &pattern_table = BuildPatternTable(pattern);
  for (int i = 0, j = 0; i < src.size(); i++) {
    // while (j > 0 && src[i] != src[j]) {
    //   j = pattern_table[j - 1];
    // }
    // if (src[i] == src[j]) {
    //   j++;
    // }
    // if (j == pattern.size()) {
    //   return true;
    // }
    if (src[i] == pattern[j]) {
      i++;
      j++;
      if (j == pattern.size()) {
        return true;
      }
    } else {
      if (j > 0) {
        j = pattern_table[j - 1];
      } else {
        i++;
      }
    }
  }

  return false;
}