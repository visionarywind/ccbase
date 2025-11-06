/**
 *
给你一个整数 n ，请你找出并返回第 n 个 丑数 。

丑数 就是质因子只包含 2、3 和 5 的正整数。



示例 1：
输入：n = 10
输出：12
解释：[1, 2, 3, 4, 5, 6, 8, 9, 10, 12] 是由前 10 个丑数组成的序列。
示例 2：
输入：n = 1
输出：1
解释：1 通常被视为丑数。


提示：
1 <= n <= 1690
 *
 */
#include <vector>
using namespace std;

class Solution {
 public:
  int nthUglyNumber(int n) {
    vector<int> res(n, 0);
    res[0] = 1;
    int a = 0, b = 0, c = 0;
    for (int i = 1; i < n; i++) {
      int na = 2 * res[a], nb = 3 * res[b], nc = 5 * res[c];
      res[i] = min(na, min(nb, nc));
      if (na == res[i]) {
        a++;
      }
      if (nb == res[i]) {
        b++;
      }
      if (nc == res[i]) {
        c++;
      }
    }
    return res[n - 1];
  }
};