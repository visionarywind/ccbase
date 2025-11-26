/**
 *
给你一个整数数组 nums。

如果 nums 的一个 子数组 中 没有逆序对 ，即不存在满足 i < j 且 nums[i] > nums[j] 的下标对，则该子数组被称为 稳定 子数组。

同时给你一个长度为 q 的 二维整数数组 queries，其中每个 queries[i] = [li, ri] 表示一个查询。对于每个查询 [li,
ri]，请你计算完全包含在 nums[li..ri] 内的 稳定子数组 的数量。

返回一个长度为 q 的整数数组 ans，其中 ans[i] 是第 i 个查询的答案。

注意：
子数组 是数组中一个连续且 非空 的元素序列。
单个元素的子数组被认为是稳定的。

示例 1：
输入：nums = [3,1,2], queries = [[0,1],[1,2],[0,2]]
输出：[2,3,4]
解释：
对于 queries[0] = [0, 1]，子数组为 [nums[0], nums[1]] = [3, 1]。
稳定子数组包括 [3] 和 [1]。稳定子数组的总数为 2。
对于 queries[1] = [1, 2]，子数组为 [nums[1], nums[2]] = [1, 2]。
稳定子数组包括 [1]、[2] 和 [1, 2]。稳定子数组的总数为 3。
对于 queries[2] = [0, 2]，子数组为 [nums[0], nums[1], nums[2]] = [3, 1, 2]。
稳定子数组包括 [3]、[1]、[2] 和 [1, 2]。稳定子数组的总数为 4。
因此，ans = [2, 3, 4]。

示例 2：
输入：nums = [2,2], queries = [[0,1],[0,0]]
输出：[3,1]
解释：
对于 queries[0] = [0, 1]，子数组为 [nums[0], nums[1]] = [2, 2]。
稳定子数组包括 [2]、[2] 和 [2, 2]。稳定子数组的总数为 3。
对于 queries[1] = [0, 0]，子数组为 [nums[0]] = [2]。
稳定子数组包括 [2]。稳定子数组的总数为 1。
因此，ans = [3, 1]。


提示：
1 <= nums.length <= 105
1 <= nums[i] <= 105
1 <= queries.length <= 105
queries[i] = [li, ri]
0 <= li <= ri <= nums.length - 1
 *
 */
#include <vector>
using namespace std;

class Solution {
 public:
  vector<long long> countStableSubarrays(vector<int> &nums, vector<vector<int>> &queries) {
    auto calc = [](int m) { return (long long)m * (m + 1) / 2; };
    int len = nums.size();
    vector<int> left;
    vector<long long> acc{0};
    int idx = 0;
    for (int i = 0; i < len; i++) {
      if (i == len - 1 || nums[i] > nums[i + 1]) {
        left.emplace_back(idx);
        long long  cnt = calc(i - idx + 1);
        acc.emplace_back(acc.back() + cnt);
        idx = i + 1;
      }
    }
    auto upper_bound = [&](int e) {
      int l = 0, r = left.size();
      while (l < r) {
        int m = l + ((r - l) >> 1);
        if (left[m] <= e) {
          l = m + 1;
        } else {
          r = m;
        }
      }
      return r;
    };
    vector<long long> ans(queries.size());
    for (int i = 0; i < queries.size(); i++) {
      auto &query = queries[i];
      auto start = upper_bound(query[0]);
      auto end = upper_bound(query[1]);
      if (start == end) {
        ans[i] = (calc(query[1] - query[0] + 1));
        continue;
      }
      long long l = calc(left[start] - query[0]);
      long long r = calc(query[1] - left[end - 1] + 1);
      ans[i] = (l + (acc[end - 1] - acc[start]) + r);
    }
    return ans;
  }
};