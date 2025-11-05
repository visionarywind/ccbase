/**
 *
给你一个 只包含正整数 的 非空 数组 nums 。请你判断是否可以将这个数组分割成两个子集，使得两个子集的元素和相等。



示例 1：

输入：nums = [1,5,11,5]
输出：true
解释：数组可以分割成 [1, 5, 5] 和 [11] 。
示例 2：

输入：nums = [1,2,3,5]
输出：false
解释：数组不能分割成两个元素和相等的子集。


提示：

1 <= nums.length <= 200
1 <= nums[i] <= 100
 *
*/
#include <vector>
using namespace std;

class Solution {
 public:
  bool canPartition(vector<int> &nums) {
    int len = nums.size();
    if (len == 1) {
      return false;
    }
    int sum = 0;
    for (int i = 0; i < len; i++) {
      sum += nums[i];
    }
    if ((sum & 1) == 1) {
      return false;
    }
    int target = sum >> 1;
    vector<vector<bool>> dp(len + 1, vector<bool>(target + 1, false));  // i个元素是否能够凑够j个背包
    // dp[i][j] = dp[i - 1][j - nums[i]] | dp[i - 1][j]
    for (int i = 0; i <= len; i++) {
      dp[i][0] = true;
    }
    for (int i = 0; i < len; i++) {
      for (int j = 1; j <= target; j++) {
        if (j >= nums[i]) {
          dp[i + 1][j] = dp[i][j - nums[i]] | dp[i - 1][j];
        } else {
          dp[i + 1][j] = dp[i][j];
        }
      }
    }
    return dp[len][target];
  }
};