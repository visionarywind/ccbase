#include <vector>
using namespace std;

class Solution {
 public:
  int minimumTotal(vector<vector<int>> &triangle) {
    if (triangle.size() == 1) {
      return triangle[0][0];
    }
    vector<int> dp(triangle[triangle.size() - 1].size(), 0);
    dp[0] = triangle[0][0];
    for (int i = 1; i < triangle.size(); i++) {
      int levelLen = triangle[i].size();
      dp[levelLen - 1] = dp[levelLen - 2] + triangle[i][levelLen - 1];

      for (int j = levelLen - 2; j > 0; j--) {
        dp[j] = min(dp[j - 1], dp[j]) + triangle[i][j];
      }

      dp[0] += triangle[i][0];
    }
    int ans = 10001;
    for (int i = 0; i < dp.size(); i++) {
      ans = min(ans, dp[i]);
    }
    return ans;
  }

  int minimumTotal2(vector<vector<int>> &triangle) {
    if (triangle.empty()) {
      return 0;
    }
    if (triangle.size() == 1) {
      return triangle[0][0];
    }

    vector<int> dp(triangle.back().begin(), triangle.back().end());

    for (int i = triangle.size() - 2; i >= 0; i--) {
      for (int j = 0; j <= i; j++) {
        dp[j] = min(dp[j], dp[j + 1]) + triangle[i][j];
      }
    }

    return dp[0];
  }
};