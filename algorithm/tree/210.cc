/**
 *
给定一个二叉搜索树的根节点 root ，和一个整数 k ，请你设计一个算法查找其中第 k 小的元素（从 1 开始计数）。

示例 1：
输入：root = [3,1,4,null,2], k = 1
输出：1
示例 2：
输入：root = [5,3,6,2,4,null,null,1], k = 3
输出：3

提示：
树中的节点数为 n 。
1 <= k <= n <= 104
0 <= Node.val <= 104

进阶：如果二叉搜索树经常被修改（插入/删除操作）并且你需要频繁地查找第 k 小的值，你将如何优化算法？
 *
*/

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
#include <stack>
using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class SolutionRecursive {
 public:
  int kthSmallest(TreeNode *root, int k) {
    inOrder(root, k);
    return ans;
  }

  void inOrder(TreeNode *node, int k) {
    if (node == nullptr || ans != -1) { return; }
    inOrder(node->left, k);
    if (++cnt == k) {
      ans = node->val;
      return;
    }
    inOrder(node->right, k);
  }

  int cnt{0};
  int ans{-1};
};

class Solution {
 public:
  int kthSmallest(TreeNode *root, int k) {
    int ans;
    stack<TreeNode *> stk;
    stk.push(root);
    TreeNode *cur = root;
    while (cur != nullptr || !stk.empty()) {
      while (cur != nullptr) {
        stk.push(cur);
        cur = cur->left;
      }
      TreeNode *node = stk.top();
      stk.pop();
      if (node == nullptr) { continue; }
      if (--k == 0) {
        ans = node->val;
        break;
      }
      cur = node->right;
    }
    return ans;
  }
};