/**
 *
给你一个链表数组，每个链表都已经按升序排列。

请你将所有链表合并到一个升序链表中，返回合并后的链表。



示例 1：

输入：lists = [[1,4,5],[1,3,4],[2,6]]
输出：[1,1,2,3,4,4,5,6]
解释：链表数组如下：
[
  1->4->5,
  1->3->4,
  2->6
]
将它们合并到一个有序链表中得到。
1->1->2->3->4->4->5->6
示例 2：

输入：lists = []
输出：[]
示例 3：

输入：lists = [[]]
输出：[]


提示：

k == lists.length
0 <= k <= 10^4
0 <= lists[i].length <= 500
-10^4 <= lists[i][j] <= 10^4
lists[i] 按 升序 排列
lists[i].length 的总和不超过 10^4
 *
*/
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
#include <queue>
#include <vector>
using namespace std;

struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
 public:
  ListNode *mergeKLists(vector<ListNode *> &lists) {
    auto comparator = [](ListNode *left, ListNode *right) { return left->val > right->val; };
    priority_queue<ListNode *, vector<ListNode *>, decltype(comparator)> q(comparator);
    for (auto list : lists) {
      if (list != nullptr) {
        q.push(list);
      }
    }
    ListNode dummyHead;
    ListNode *node = &dummyHead;
    while (!q.empty()) {
      ListNode *head = q.top();
      q.pop();
      node->next = head;
      node = node->next;

      head = head->next;
      if (head != nullptr) {
        q.push(head);
      }
    }
    return dummyHead.next;
  }

  ListNode *mergeKListsBinary2(vector<ListNode *> &lists) {
    if (lists.size() == 0) {
      return nullptr;
    }
    return merge(lists, 0, lists.size() - 1);
  }

  ListNode *merge(vector<ListNode *> &lists, int left, int right) {
    if (left == right) {
      return lists[left];
    }
    int mid = left + ((right - left) >> 1);
    return merge2List(merge(lists, left, mid), merge(lists, mid + 1, right));
  }

  ListNode *mergeKListsBinary(vector<ListNode *> &lists) {
    vector<ListNode *> ans = lists;
    do {
      ans = binaryMerge(ans);
    } while (ans.size() > 1);
    return ans.size() > 0 ? ans[0] : nullptr;
  }

  vector<ListNode *> binaryMerge(vector<ListNode *> &lists) {
    if (lists.size() < 2) {
      return lists;
    }
    vector<ListNode *> ans;
    int i = 0;
    for (; i < lists.size() + 1; i += 2) {
      ans.push_back(merge2List(lists[i], lists[i + 1]));
    }
    if (i < lists.size()) {
      ans.push_back(lists[lists.size() - 1]);
    }
    return ans;
  }

  ListNode *mergeKListsDirect1(vector<ListNode *> &lists) {
    ListNode *ans = nullptr;
    for (auto list : lists) {
      ans = merge2List(ans, list);
    }
    return ans;
  }

  ListNode *merge2List(ListNode *left, ListNode *right) {
    ListNode dummyHead;
    ListNode *node = &dummyHead;
    while (left != nullptr && right != nullptr) {
      if (left->val <= right->val) {
        node->next = left;
        left = left->next;
      } else {
        node->next = right;
        right = right->next;
      }
      node = node->next;
    }
    if (left != nullptr) {
      node->next = left;
    } else {
      node->next = right;
    }
    return dummyHead.next;
  }
};