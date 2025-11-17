/**
 *
n 对情侣坐在连续排列的 2n 个座位上，想要牵到对方的手。

人和座位由一个整数数组 row 表示，其中 row[i] 是坐在第 i 个座位上的人的 ID。情侣们按顺序编号，第一对是 (0, 1)，第二对是
(2, 3)，以此类推，最后一对是 (2n - 2, 2n - 1)。

返回 最少交换座位的次数，以便每对情侣可以并肩坐在一起。 每次交换可选择任意两人，让他们站起来交换座位。



示例 1:
输入: row = [0,2,1,3]
输出: 1
解释: 只需要交换第二个人（row[1]）和第三个人（row[2]）的位置即可。
示例 2:
输入: row = [3,2,0,1]
输出: 0
解释: 无需交换座位，所有的情侣都已经可以手牵手了。


提示:
2n == row.length
2 <= n <= 30
n 是偶数
0 <= row[i] < 2n
row 中所有元素均 无重复
 *
 */
#include <functional>
#include <numeric>
#include <vector>
using namespace std;

class Solution {
 public:
  int minSwapsCouples(vector<int> &row) {
    int len = row.size() >> 1;
    vector<int> parent(len);
    iota(parent.begin(), parent.end(), 0);
    int ans = 0;
    for (int i = 0; i < row.size(); i += 2) {
      int l = row[i] >> 1, r = row[i + 1] >> 1;
      int left = find(parent, l), right = find(parent, r);
      if (left != right) {
        parent[left] = right;
        ans++;
      }
    }
    return ans;
  }

  int find(vector<int> &parent, int x) {
    if (x != parent[x]) {
      parent[x] = find(parent, parent[x]);
    }
    return parent[x];
  }
};