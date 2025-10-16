#include <string>
#include <vector>
/**
 * Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses.
  Example 1:

  Input: n = 3
  Output: ["((()))","(()())","(())()","()(())","()()()"]
  Example 2:

  Input: n = 1
  Output: ["()"]
*/
void backtrace(std::vector<std::string> *ans, size_t n, size_t left, size_t right, std::string result) {
  if (left < right || left > n - 1) { return; }
  if (left == right && left == n - 1) {
    ans->push_back(result);
    return;
  }
  backtrace(ans, n, left + 1, right, result + "(");
  backtrace(ans, n, left, right + 1, result + ")");
}

std::vector<std::string> generate(size_t n) {
  std::vector<std::string> ans;
  backtrace(&ans, n, 0, 0, "");
  return ans;
}