#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;
/**
 * 给定一个不含重复数字的数组 nums ，返回其 所有可能的全排列 。你可以 按任意顺序 返回答案。
    示例 1：

    输入：nums = [1,2,3]
    输出：[[1,2,3],[1,3,2],[2,1,3],[2,3,1],[3,1,2],[3,2,1]]
    示例 2：

    输入：nums = [0,1]
    输出：[[0,1],[1,0]]
    示例 3：

    输入：nums = [1]
    输出：[[1]]


    提示：
    1 <= nums.length <= 6
    -10 <= nums[i] <= 10
    nums 中的所有整数 互不相同
 * 
*/
void backtrace(vector<vector<int>> *ans, const vector<int> &nums, bool *flag, vector<int> *temp) {
  if (temp->size() == nums.size()) {
    ans->push_back(*temp);
    return;
  }
  for (int i = 0; i < nums.size(); i++) {
    if (flag[i]) {
      continue;
    }
    flag[i] = true;
    temp->emplace_back(nums[i]);
    backtrace(ans, nums, flag, temp);
    flag[i] = false;
    temp->pop_back();
  }
}

vector<vector<int>> permutation(const vector<int> &nums) {
  vector<vector<int>> ans;
  vector<int> temp;
  bool *flag = new bool[nums.size()];
  memset(flag, 0, nums.size());
  backtrace(&ans, nums, flag, &temp);
  delete[] flag;
  return ans;
}

/**
 * 升级版 数字可能重复
*/
void backtrace2(vector<vector<int>> *ans, const vector<int> &nums, bool *flag, vector<int> *temp) {
  if (temp->size() == nums.size()) {
    ans->push_back(*temp);
    return;
  }
  // cout << "temp : " << temp->size() << endl;
  for (int i = 0; i < nums.size(); i++) {
    // !flag[i - 1] means 当前值和上一个值相同，上一个值未被使用，保证相同值在最终结果中的相对顺序
    if (flag[i] || (i > 0 && nums[i] == nums[i - 1] && !flag[i - 1])) {
      continue;
    }
    flag[i] = true;
    temp->emplace_back(nums[i]);
    backtrace2(ans, nums, flag, temp);
    flag[i] = false;
    temp->pop_back();
  }
}

vector<vector<int>> permutation2(vector<int> &nums) {
  vector<vector<int>> ans;
  vector<int> temp;
  bool *flag = new bool[nums.size()];
  memset(flag, 0, nums.size());
  std::sort(nums.begin(), nums.end());
  backtrace2(&ans, nums, flag, &temp);
  delete[] flag;
  return ans;
}

int main() {
  vector<int> input = {1, 2, 3, 1};
  auto ret = permutation2(input);
  for (int i = 0; i < ret.size(); i++) {
    auto arr = ret[i];
    for (int j = 0; j < arr.size(); j++) {
      cout << arr[j] << " ";
    }
    cout << endl;
  }
  return 0;
}
