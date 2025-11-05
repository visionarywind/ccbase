/**
 *
给你一个由 不同 整数组成的数组 nums ，和一个目标整数 target 。请你从 nums 中找出并返回总和为 target 的元素组合的个数。

题目数据保证答案符合 32 位整数范围。



示例 1：

输入：nums = [1,2,3], target = 4
输出：7
解释：
所有可能的组合为：
(1, 1, 1, 1)
(1, 1, 2)
(1, 2, 1)
(1, 3)
(2, 1, 1)
(2, 2)
(3, 1)
请注意，顺序不同的序列被视作不同的组合。
示例 2：

输入：nums = [9], target = 3
输出：0


提示：

1 <= nums.length <= 200
1 <= nums[i] <= 1000
nums 中的所有元素 互不相同
1 <= target <= 1000
 *
*/
#include <vector>
using namespace std;

class Solution {
 public:
  int combinationSum4(vector<int> &nums, int target) {
    int len = nums.size();
    vector<vector<int>> dp(len + 1, vector<int>(target + 1, 0));
    for (int i = 0; i <= len; i++) {
      dp[i][0] = 1;
    }
    for (int j = 1; j <= target; j++) {
      for (int i = 1; i <= len; i++) {
        // dp[i][j] = { dp[i - 1][j] + dp[i - 1][j - nums[i]] }
        if (j >= nums[i - 1] && dp[i - 1][j] <= INT32_MAX - dp[len][j - nums[i - 1]]) {
          // dp[len][j - nums[i - 1]]
          dp[i][j] = dp[i - 1][j] + dp[len][j - nums[i - 1]];
        } else {
          dp[i][j] = dp[i - 1][j];
        }
      }
    }
    return dp[len][target];
  }
};