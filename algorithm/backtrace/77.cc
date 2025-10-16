/**
 * 给定两个整数 n 和 k，返回范围 [1, n] 中所有可能的 k 个数的组合。

    你可以按 任何顺序 返回答案。

    

    示例 1：

    输入：n = 4, k = 2
    输出：
    [
      [2,4],
      [3,4],
      [2,3],
      [1,2],
      [1,3],
      [1,4],
    ]
    示例 2：

    输入：n = 1, k = 1
    输出：[[1]] 
  */
#include <vector>

std::vector<std::vector<int>> backtrace(std::vector<std::vector<int>> *ans, int n, int k, int index, std::vector<int> temp) {
  if (temp.size() == k) {
    ans->push_back(temp);
    return;
  }
  for (int i = index; i <= n; i++) {
    temp.push_back(i);
    backtrace(ans, n, k, i + 1, temp);
    temp.pop_back();
  }
}
std::vector<std::vector<int>> combinations(int n, int k) {
  std::vector<std::vector<int>> ans;
  backtrace(&ans, n, k, 1, std::vector<int>());
  return ans;
}
