#include <vector>
using namespace std;

class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> ans;
        vector<int> temp;
        backtrace(nums, 0, &ans, &temp);
        return ans;
    }

private:
    void backtrace(vector<int> &nums, int index, vector<vector<int>> *ans, vector<int> *temp) {
        ans->emplace_back(*temp);
        if (index >= nums.size()) { return; }
        for (int i = index; i < nums.size(); i++) {
            temp->emplace_back(nums[i]);
            backtrace(nums, i + 1, ans, temp);
            temp->pop_back();
        }
    }
};