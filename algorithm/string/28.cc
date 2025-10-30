#include <string>
#include <vector>
using namespace std;

class Solution {
 public:
  int strStr(string haystack, string needle) {
    if (needle.empty() || needle.size() > haystack.size()) {
      return -1;
    }
    const auto &advance = next(needle);
    int j = 0;
    for (int i = 0; i < haystack.length(); i++) {
      while (j > 0 && haystack[i] != needle[j]) {
        j = advance[j - 1];
      }
      if (haystack[i] == needle[j]) {
        j++;
      }
      if (j == needle.length()) {
        return i - j + 1;
      }
    }
    return -1;
  }

 private:
  vector<int> next(string needle) {
    vector<int> ans(needle.length(), 0);
    int j = 0;
    for (int i = 1; i < needle.length(); i++) {
      while (j > 0 && needle[i] != needle[j]) {
        j = ans[j - 1];
      }
      if (needle[i] == needle[j]) {
        j++;
      }
      ans[i] = j;
    }
    return ans;
  }
};