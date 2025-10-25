/**
 *
示例 1：

输入：intervals = [[1,3],[6,9]], newInterval = [2,5]
输出：[[1,5],[6,9]]

示例 2：

输入：intervals = [[1,2],[3,5],[6,7],[8,10],[12,16]], newInterval = [4,8]
输出：[[1,2],[3,10],[12,16]]
解释：这是因为新的区间 [4,8] 与 [3,5],[6,7],[8,10] 重叠。
 *
*/
#include <algorithm>
#include <vector>
using namespace std;

class Solution {
 public:
  vector<vector<int>> insert(vector<vector<int>> &intervals, vector<int> &newInterval) {
    vector<vector<int>> ans;
    int left = newInterval[0];
    int right = newInterval[1];
    bool flag = false;
    for (auto &interval : intervals) {
      if (interval[1] < left) {
        ans.push_back(interval);
      } else if (interval[0] > right) {
        if (!flag) {
          ans.push_back({left, right});
          flag = true;
        }
        ans.push_back(interval);
      } else {
        left = min(left, interval[0]);
        right = max(right, interval[1]);
      }
    }
    if (!flag) {
      ans.push_back({left, right});
    }
    return ans;
  }
};
