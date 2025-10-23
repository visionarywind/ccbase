/**
 *
给定 n 个非负整数表示每个宽度为 1 的柱子的高度图，计算按此排列的柱子，下雨之后能接多少雨水。

示例 1：
  输入：height = [0,1,0,2,1,0,1,3,2,1,2,1]
  输出：6
  解释：上面是由数组 [0,1,0,2,1,0,1,3,2,1,2,1] 表示的高度图，在这种情况下，可以接 6 个单位的雨水（蓝色部分表示雨水）。

示例 2：
  输入：height = [4,2,0,3,2,5]
  输出：9

提示：
  n == height.length
  1 <= n <= 2 * 104
  0 <= height[i] <= 105
 *
*/
#include <stack>
#include <vector>
using namespace std;

class Solution {
 public:
  int trap(vector<int> &height) {
    int ans = 0;
    stack<int> stk;
    for (int i = 0; i < height.size(); i++) {
      while (!stk.empty() && height[i] > height[stk.top()]) {
        int top = height[stk.top()];
        stk.pop();
        if (stk.empty()) {
          break;
        }
        int range = i - stk.top() - 1;
        ans += range * (min(height[stk.top()], height[i]) - top);
      }
      stk.push(i);
    }
    return ans;
  }

  int trapTwoPointer(vector<int> &height) {
    int ans = 0;
    int left = 0;
    int right = height.size() - 1;
    int leftMax = 0;
    int rightMax = 0;
    while (left < right) {
      leftMax = max(leftMax, height[left]);
      rightMax = max(rightMax, height[right]);
      if (height[left] < height[right]) {
        ans += leftMax - height[left];
        left++;
      } else {
        ans += rightMax - height[right];
        right--;
      }
    }
    return ans;
  }

  int trapDp(vector<int> &height) {
    vector<int> leftMax(height.size());
    leftMax[0] = height[0];
    for (int i = 1; i < height.size(); i++) {
      leftMax[i] = max(leftMax[i - 1], height[i]);
    }

    vector<int> rightMax(height.size());
    rightMax[height.size() - 1] = height[height.size() - 1];
    for (int i = height.size() - 2; i > -1; i--) {
      rightMax[i] = max(rightMax[i + 1], height[i]);
    }

    int ans = 0;
    for (int i = 0; i < height.size(); i++) {
      ans += min(rightMax[i], leftMax[i]) - height[i];
    }
    return ans;
  }
};