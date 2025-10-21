#include <vector>
using namespace std;

class Solution {
 public:
  int maxArea(vector<int> &height) {
    int ans = 0;
    if (height.size() <= 1) {
      return ans;
    }
    int left = 0;
    int right = height.size() - 1;
    while (left < right) {
      int rectangle = std::min(height[left], height[right]) * (right - left);
      ans = std::max(ans, rectangle);
      if (height[left] < height[right]) {
        left++;
      } else {
        right--;
      }
    }
    return ans;
  }
};