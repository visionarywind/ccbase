#include <cctype>
#include <string_view>
#include <vector>

std::vector<size_t> BuildPatternTable(const std::string_view &pattern) {
  std::vector<size_t> pattern_table(pattern.size(), 0);
  for (size_t i = 1, j = 0; i < pattern.size();) {
    if (pattern[i] == pattern[j]) {
      j++;
      pattern_table[i] = j;
      i++;
    } else {
      if (j > 0) {
        j = pattern_table[j - 1];
      } else {
        i++;
      }
    }
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
  for (size_t i = 0, j = 0; i < src.size();) {
    if (src[i] == pattern[j]) {
      j++;
      if (j == pattern.size()) {
        return true;
      }
      i++;
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