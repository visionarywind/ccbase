/**
 *
给你一个整数数组 nums ，找出并返回所有该数组中不同的递增子序列，递增子序列中 至少有两个元素 。你可以按 任意顺序
返回答案。

数组中可能含有重复元素，如出现两个整数相等，也可以视作递增序列的一种特殊情况。



示例 1：

输入：nums = [4,6,7,7]
输出：[[4,6],[4,6,7],[4,6,7,7],[4,7],[4,7,7],[6,7],[6,7,7],[7,7]]
示例 2：

输入：nums = [4,4,3,2,1]
输出：[[4,4]]


提示：

1 <= nums.length <= 15
-100 <= nums[i] <= 100
 *
 */
#include <vector>
#include <unordered_set>
using namespace std;

class Solution {
 public:
  vector<vector<int>> findSubsequences(vector<int> &nums) {
    vector<vector<int>> ans;
    vector<int> path;
    backtrace(nums, 0, ans, path);
    return ans;
  }

  void backtrace(vector<int> &nums, int idx, vector<vector<int>> &ans, vector<int> &path) {
    if (path.size() > 1) {
      ans.emplace_back(path);
    }
    unordered_set<int> used;
    for (int i = idx; i < nums.size(); i++) {
      if (used.count(nums[i]) != 0) {
        continue;
      }
      if (!path.empty() && path.back() > nums[i]) {
        continue;
      }
      used.insert(nums[i]);
      path.emplace_back(nums[i]);
      backtrace(nums, i + 1, ans, path);
      path.pop_back();
    }
  }
};