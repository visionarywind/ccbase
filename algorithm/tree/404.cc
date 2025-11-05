/**
 *
给定二叉树的根节点 root ，返回所有左叶子之和。



示例 1：



输入: root = [3,9,20,null,null,15,7]
输出: 24
解释: 在这个二叉树中，有两个左叶子，分别是 9 和 15，所以返回 24
示例 2:

输入: root = [1]
输出: 0


提示:

节点数在 [1, 1000] 范围内
-1000 <= Node.val <= 1000
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
#include <iostream>
using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
 public:
  int sumOfLeftLeaves(TreeNode *root) {
    if (root == nullptr || (root->left == nullptr && root->right == nullptr)) {
      return 0;
    }
    int leftVal = 0;
    if (root->left != nullptr && root->left->left == nullptr && root->left->right == nullptr) {
      leftVal = root->left->val;
    } else {
      leftVal = sumOfLeftLeaves(root->left);
    }
    return leftVal + sumOfLeftLeaves(root->right);
  }
};