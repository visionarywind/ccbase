/**
 *
给你一个整数数组 prices 和一个整数 k ，其中 prices[i] 是某支给定的股票在第 i 天的价格。

设计一个算法来计算你所能获取的最大利润。你最多可以完成 k 笔交易。也就是说，你最多可以买 k 次，卖 k 次。

注意：你不能同时参与多笔交易（你必须在再次购买前出售掉之前的股票）。



示例 1：

输入：k = 2, prices = [2,4,1]
输出：2
解释：在第 1 天 (股票价格 = 2) 的时候买入，在第 2 天 (股票价格 = 4) 的时候卖出，这笔交易所能获得利润 = 4-2 = 2 。
示例 2：

输入：k = 2, prices = [3,2,6,5,0,3]
输出：7
解释：在第 2 天 (股票价格 = 2) 的时候买入，在第 3 天 (股票价格 = 6) 的时候卖出, 这笔交易所能获得利润 = 6-2 = 4 。
     随后，在第 5 天 (股票价格 = 0) 的时候买入，在第 6 天 (股票价格 = 3) 的时候卖出, 这笔交易所能获得利润 = 3-0 = 3 。


提示：

1 <= k <= 100
1 <= prices.length <= 1000
0 <= prices[i] <= 1000
 *
 */
#include <vector>
using namespace std;

class Solution {
 public:
  int maxProfit(int k, vector<int> &prices) {
    int len = prices.size();
    // hold[i][j] 表示第i天持有股票，已完成j次交易的最大利润
    // 可以从前一天持有股票的状态转移过来，或者今天买入(需要前一次交易完成后)
    vector<vector<int>> hold(len, vector<int>(k, 0));
    // rest[i][j] 表示第i天不持有股票，已完成j次交易的最大利润
    // 可以从前一天不持有股票的状态转移过来，或者今天卖出(基于当前持有状态)
    vector<vector<int>> rest(len, vector<int>(k, 0));
    for (int i = 0; i < k; i++) {
      hold[0][i] = -prices[0];
    }
    for (int i = 1; i < len; i++) {
      for (int j = 0; j < k; j++) {
        hold[i][j] = max(hold[i - 1][j], (j > 0 ? rest[i - 1][j - 1] : 0) - prices[i]);
        rest[i][j] = max(rest[i - 1][j], hold[i - 1][j] + prices[i]);
      }
    }
    return rest[len - 1][k - 1];
  }
};
