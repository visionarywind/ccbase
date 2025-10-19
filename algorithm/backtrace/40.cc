class Solution {
 public:
  vector<vector<int>> combinationSum2(vector<int> &candidates, int target) {
    vector<vector<int>> ans;
    vector<int> temp;
    std::sort(candidates.begin(), candidates.end());
    backtrace(candidates, target, 0, 0, &ans, &temp);
    return ans;
  }

 private:
  void backtrace(vector<int> &candidates, int target, int index, int sum, vector<vector<int>> *ans, vector<int> *temp) {
    if (sum == target) {
      ans->emplace_back(*temp);
      return;
    }
    if (index >= candidates.size() || sum > target) {
      return;
    }
    for (int i = index; i < candidates.size(); i++) {
      if (i > index && candidates[i - 1] == candidates[i]) {
        continue;
      }
      temp->emplace_back(candidates[i]);
      backtrace(candidates, target, i + 1, sum + candidates[i], ans, temp);
      temp->pop_back();
    }
  }
};