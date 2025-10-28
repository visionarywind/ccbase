/**
 *
给定两个整数数组 inorder 和 postorder ，其中 inorder 是二叉树的中序遍历， postorder
是同一棵树的后序遍历，请你构造并返回这颗 二叉树 。



示例 1:


输入：inorder = [9,3,15,20,7], postorder = [9,15,7,20,3]
输出：[3,9,20,null,null,15,7]
示例 2:

输入：inorder = [-1], postorder = [-1]
输出：[-1]


提示:

1 <= inorder.length <= 3000
postorder.length == inorder.length
-3000 <= inorder[i], postorder[i] <= 3000
inorder 和 postorder 都由 不同 的值组成
postorder 中每一个值都在 inorder 中
inorder 保证是树的中序遍历
postorder 保证是树的后序遍历
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

#include <vector>
#include <queue>
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
  TreeNode *buildTree(vector<int> &inorder, vector<int> &postorder) {
    if (inorder.empty() || inorder.size() != postorder.size()) {
      return nullptr;
    }
    return helper(inorder, 0, inorder.size() - 1, postorder, 0, postorder.size() - 1);
  }

  TreeNode *helper(vector<int> &inorder, int ileft, int iright, vector<int> &postorder, int pstart, int pend) {
    int rootVal = postorder[pend];
    TreeNode *root = new TreeNode(rootVal);
    int index = ileft;
    while (index <= iright && inorder[index] != rootVal) {
      index++;
    }
    int leftLen = index - ileft;
    TreeNode *left = leftLen > 0 ? helper(inorder, ileft, index - 1, postorder, pstart, pstart + leftLen - 1) : nullptr;
    root->left = left;

    int rightLen = iright - index;
    TreeNode *right = rightLen > 0 ? helper(inorder, index + 1, iright, postorder, pend - rightLen, pend - 1) : nullptr;
    root->right = right;
    return root;
  }
};
