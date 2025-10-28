/**
 * 
给你一个整数 n ，求恰由 n 个节点组成且节点值从 1 到 n 互不相同的 二叉搜索树 有多少种？返回满足题意的二叉搜索树的种数。

 

示例 1：


输入：n = 3
输出：5
示例 2：

输入：n = 1
输出：1
 

提示：

1 <= n <= 19
 * 
*/
#include <unordered_map>
using namespace std;

class Solution {
 public:
  int numTrees(int n) {
    if (cache_.count(n) != 0) {
      return cache_[n];
    }
    if (n <= 2) {
      cache_[n] = n;
      return n;
    }
    int ans = 0;
    for (int i = 1; i <= n; i++) {
      ans += (i > 1 ? numTrees(i - 1) : 1) * (n - i > 0 ? numTrees(n - i) : 1);
    }
    cache_[n] = ans;
    return ans;
  }

 private:
  unordered_map<int, long> cache_;
};