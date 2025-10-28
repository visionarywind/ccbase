/**
 *
序列化是将一个数据结构或者对象转换为连续的比特位的操作，进而可以将转换后的数据存储在一个文件或者内存中，同时也可以通过网络传输到另一个计算机环境，采取相反方式重构得到原数据。

请设计一个算法来实现二叉树的序列化与反序列化。这里不限定你的序列 /
反序列化算法执行逻辑，你只需要保证一个二叉树可以被序列化为一个字符串并且将这个字符串反序列化为原始的树结构。

提示: 输入输出格式与 LeetCode 目前使用的方式一致，详情请参阅 LeetCode
序列化二叉树的格式。你并非必须采取这种方式，你也可以采用其他的方法解决这个问题。



示例 1：


输入：root = [1,2,3,null,null,4,5]
输出：[1,2,3,null,null,4,5]
示例 2：

输入：root = []
输出：[]
示例 3：

输入：root = [1]
输出：[1]
示例 4：

输入：root = [1,2]
输出：[1,2]


提示：

树中结点数在范围 [0, 104] 内
-1000 <= Node.val <= 1000
 *
*/

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Codec {
 public:
  // Encodes a tree to a single string.
  string serialize(TreeNode *root) {
    if (root == nullptr) {
      return "null";
    }
    return to_string(root->val) + "," + serialize(root->left) + "," + serialize(root->right);
  }

  // Decodes your encoded data to tree.
  TreeNode *deserialize(string data) {
    queue<string> elements = parse(data);
    return helper(elements);
  }

  TreeNode *helper(queue<string> &elements) {
    auto element = elements.front();
    elements.pop();
    if (element == "null") {
      return nullptr;
    }
    TreeNode *root = new TreeNode(stoi(element));
    root->left = helper(elements);
    root->right = helper(elements);
    return root;
  }

  queue<string> parse(string data) {
    queue<string> res;
    size_t start = 0;
    size_t index = data.find(",", start);
    while (index != std::string::npos) {
      res.push(data.substr(start, index - start));
      start = index + 1;
      index = data.find(",", start);
    }
    res.push(data.substr(start));
    return res;
  }
};

// Your Codec object will be instantiated and called as such:
// Codec ser, deser;
// TreeNode* ans = deser.deserialize(ser.serialize(root));

int main() {
  Codec codec;
  // [1,2,3,null,null,4,5]
  TreeNode *root = new TreeNode(1);
  root->left = new TreeNode(2);
  root->right = new TreeNode(3);
  auto node = root->right;
  node->left = new TreeNode(4);
  node->right = new TreeNode(5);
  auto str = codec.serialize(root);
  cout << str << endl;
  // auto q = codec.parse(str);
  // for (; !q.empty(); ) {
  //   cout << q.front() << endl;
  //   q.pop();
  // }
  TreeNode *ans = codec.deserialize(str);
  return 0;
}