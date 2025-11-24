/**
 *
给定一个区间的集合 intervals ，其中 intervals[i] = [starti, endi] 。返回 需要移除区间的最小数量，使剩余区间互不重叠 。

注意 只在一点上接触的区间是 不重叠的。例如 [1, 2] 和 [2, 3] 是不重叠的。



示例 1:

输入: intervals = [[1,2],[2,3],[3,4],[1,3]]
输出: 1
解释: 移除 [1,3] 后，剩下的区间没有重叠。
示例 2:

输入: intervals = [ [1,2], [1,2], [1,2] ]
输出: 2
解释: 你需要移除两个 [1,2] 来使剩下的区间没有重叠。
示例 3:

输入: intervals = [ [1,2], [2,3] ]
输出: 0
解释: 你不需要移除任何区间，因为它们已经是无重叠的了。


提示:

1 <= intervals.length <= 105
intervals[i].length == 2
-5 * 104 <= starti < endi <= 5 * 104
 *
 */
#include <vector>
using namespace std;
class Solution {
 public:
  int eraseOverlapIntervals(vector<vector<int>> &intervals) {
    sort(intervals.begin(), intervals.end(), [](const vector<int> &l, const vector<int> &r) { return l[1] - r[1]; });
    int len = intervals.size();
    int res = 1;
    int right = intervals[0][1];
    for (int i = 1; i < len; i++) {
      if (intervals[i][0] >= right) {
        right = intervals[i][1];
        res++;
      }
    }
    return len - res;
  }

  int search(vector<int>& nums, int target) {
        int l = 0, r = nums.size();
        while (l < r) {
            int m = l + ((r - l) >> 1);
            if (nums[m] == target) {
                return m;
            } else if (nums[m] < target) {
                l = m + 1;
            } else {
                r = m;
            }
        }
        return -1;
    }
};
