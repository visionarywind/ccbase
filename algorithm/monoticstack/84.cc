#include <vector>
using namespace std;

class Solution {
 public:
  int largestRectangleArea(vector<int> &heights) {
    int ans = 0;
    heights.insert(heights.begin(), 0);
    heights.emplace_back(0);
    vector<int> stk;
    for (int i = 0; i < heights.size(); i++) {
      while (!stk.empty() && heights[i] < heights[stk.back()]) {
        int idx = stk.back();
        stk.pop_back();
        int left = stk.back() + 1;
        int right = i - 1;
        ans = max(ans, heights[idx] * (right - left + 1));
      }
      stk.push_back(i);
    }
    return ans;
  }
};
