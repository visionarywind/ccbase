/**
 *
按字典 wordList 完成从单词 beginWord 到单词 endWord 转化，一个表示此过程的 转换序列 是形式上像 beginWord -> s1 -> s2 ->
... -> sk 这样的单词序列，并满足：

每对相邻的单词之间仅有单个字母不同。
转换过程中的每个单词 si（1 <= i <= k）必须是字典 wordList 中的单词。注意，beginWord 不必是字典 wordList 中的单词。
sk == endWord
给你两个单词 beginWord 和 endWord ，以及一个字典 wordList 。请你找出并返回所有从 beginWord 到 endWord 的 最短转换序列
，如果不存在这样的转换序列，返回一个空列表。每个序列都应该以单词列表 [beginWord, s1, s2, ..., sk] 的形式返回。



示例 1：

输入：beginWord = "hit", endWord = "cog", wordList = ["hot","dot","dog","lot","log","cog"]
输出：[["hit","hot","dot","dog","cog"],["hit","hot","lot","log","cog"]]
解释：存在 2 种最短的转换序列：
"hit" -> "hot" -> "dot" -> "dog" -> "cog"
"hit" -> "hot" -> "lot" -> "log" -> "cog"
示例 2：

输入：beginWord = "hit", endWord = "cog", wordList = ["hot","dot","dog","lot","log"]
输出：[]
解释：endWord "cog" 不在字典 wordList 中，所以不存在符合要求的转换序列。


提示：

1 <= beginWord.length <= 5
endWord.length == beginWord.length
1 <= wordList.length <= 500
wordList[i].length == beginWord.length
beginWord、endWord 和 wordList[i] 由小写英文字母组成
beginWord != endWord
wordList 中的所有单词 互不相同
 *
*/
#include <list>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include <queue>
using namespace std;

class Solution {
 public:
  vector<vector<string>> findLadders(string beginWord, string endWord, vector<string> &wordList) {
    vector<vector<string>> ans;
    unordered_set<string> wordSet(wordList.begin(), wordList.end());
    if (wordSet.count(endWord) == 0) {
      return ans;
    }
    wordSet.erase(beginWord);

    unordered_map<string, int> steps = {{beginWord, 0}};
    unordered_map<string, set<string>> footprints{{beginWord, {}}};

    queue<string> q;
    q.push(beginWord);
    bool found = false;
    int step = 0;

    while (!q.empty()) {
      int size = q.size();
      step++;

      for (int i = 0; i < size; i++) {
        string curStr = move(q.front());
        q.pop();

        string str = curStr;
        for (int j = 0; j < str.length(); j++) {
          char originalChar = str[j];
          for (char c = 'a'; c <= 'z'; c++) {
            if (c == originalChar) {
              continue;
            }
            str[j] = c;

            if (steps[str] == step) {
              footprints[str].insert(curStr);
            }
            if (wordSet.count(str) == 0) {
              continue;
            }

            wordSet.erase(str);
            q.push(str);
            footprints[str].insert(curStr);
            steps[str] = step;
            if (str == endWord) {
              found = true;
            }
          }
          str[j] = originalChar;  // 恢复原字符
        }
      }

      if (found) {
        break;
      }
    }

    if (found) {
      vector<string> path{endWord};
      backtrace(ans, footprints, endWord, path);
    }

    return ans;
  }

  void backtrace(vector<vector<string>> &ans, unordered_map<string, set<string>> &footprints, const string &endWord,
                 vector<string> &path) {
    if (footprints[endWord].empty()) {
      ans.push_back({path.rbegin(), path.rend()});
      return;
    }

    // 遍历当前单词可以转换到的所有单词
    for (const string &nextWord : footprints[endWord]) {
      path.push_back(nextWord);
      backtrace(ans, footprints, nextWord, path);
      path.pop_back();
    }
  }
};

// todo
class Solution0 {
 public:
  vector<vector<string>> findLadders(string beginWord, string endWord, vector<string> &wordList) {
    vector<vector<string>> res;
    // 因为需要快速判断扩展出的单词是否在 wordList 里，因此需要将 wordList 存入哈希表，这里命名为「字典」
    unordered_set<string> dict = {wordList.begin(), wordList.end()};
    // 修改以后看一下，如果根本就不在 dict 里面，跳过
    if (dict.find(endWord) == dict.end()) {
      return res;
    }
    // 特殊用例处理
    dict.erase(beginWord);

    // 第 1 步：广度优先搜索建图
    // 记录扩展出的单词是在第几次扩展的时候得到的，key：单词，value：在广度优先搜索的第几层
    unordered_map<string, int> steps = {{beginWord, 0}};
    // 记录了单词是从哪些单词扩展而来，key：单词，value：单词列表，这些单词可以变换到 key ，它们是一对多关系
    unordered_map<string, set<string>> from = {{beginWord, {}}};
    int step = 0;
    bool found = false;
    queue<string> q = queue<string>{{beginWord}};
    int wordLen = beginWord.length();
    while (!q.empty()) {
      step++;
      int size = q.size();
      for (int i = 0; i < size; i++) {
        const string currWord = move(q.front());
        string nextWord = currWord;
        q.pop();
        // 将每一位替换成 26 个小写英文字母
        for (int j = 0; j < wordLen; ++j) {
          const char origin = nextWord[j];
          for (char c = 'a'; c <= 'z'; ++c) {
            nextWord[j] = c;
            if (steps[nextWord] == step) {
              from[nextWord].insert(currWord);
            }
            if (dict.find(nextWord) == dict.end()) {
              continue;
            }
            // 如果从一个单词扩展出来的单词以前遍历过，距离一定更远，为了避免搜索到已经遍历到，且距离更远的单词，需要将它从
            // dict 中删除
            dict.erase(nextWord);
            // 这一层扩展出的单词进入队列
            q.push(nextWord);
            // 记录 nextWord 从 currWord 而来
            from[nextWord].insert(currWord);
            // 记录 nextWord 的 step
            steps[nextWord] = step;
            if (nextWord == endWord) {
              found = true;
            }
          }
          nextWord[j] = origin;
        }
      }
      if (found) {
        break;
      }
    }
    // 第 2 步：回溯找到所有解，从 endWord 恢复到 beginWord ，所以每次尝试操作 path 列表的头部
    if (found) {
      vector<string> Path = {endWord};
      backtrack(res, endWord, from, Path);
    }
    return res;
  }

  void backtrack(vector<vector<string>> &res, const string &Node, unordered_map<string, set<string>> &from,
                 vector<string> &path) {
    if (from[Node].empty()) {
      res.push_back({path.rbegin(), path.rend()});
      return;
    }
    for (const string &Parent : from[Node]) {
      path.push_back(Parent);
      backtrack(res, Parent, from, path);
      path.pop_back();
    }
  }
};

class Solution1 {
 public:
  vector<vector<string>> findLadders(string beginWord, string endWord, vector<string> &wordList) {
    unordered_set<string> search(wordList.begin(), wordList.end());  // hash 提高转换(查找)效率
    vector<vector<string>> res;
    deque<vector<string>> worker;   // 层节点容器
    worker.push_back({beginWord});  // beginWord 作为起始的根节点
    while (!worker.empty()) {
      unordered_set<string> visited;             // 一层内已转化过的 string 容器。set避免重复保存
      for (int i = worker.size(); i > 0; --i) {  // 层遍历
        auto sub = worker.front();
        worker.pop_front();      // 获取单个节点
        auto tail = sub.back();  // 获取单个节点内的最后一个 string 元素
        if (tail == endWord) {   // 是在到达是转换的终点
          res.push_back(sub);
          continue;
        }
        for (int j = 0; j < tail.size(); ++j) {  // 回溯试探 下层 节点的元素的可能性
          char temp = tail[j];                   // 单个 string 回溯保存状态
          for (char c = 'a'; c <= 'z'; ++c) {
            if (c == temp) continue;  // 忽略原始状态
            tail[j] = c;
            if (!search.count(tail)) continue;  // 未找到转换序
            visited.insert(tail);               // 找到转换序，保存 已使用 转换
            sub.push_back(tail);                // 节点 回溯
            worker.push_back(sub);              // 向容器内保存 子节点
            sub.pop_back();                     // 回溯恢复
          }
          tail[j] = temp;  // string 回溯恢复状态
        }
      }
      if (res.size()) return res;               // 第一次到达 树底，也就是得到最小转换序
      for (auto &w : visited) search.erase(w);  // 删除已使用的转换序
    }
    return {};
  }
};

class Solution2 {
 public:
  vector<vector<string>> findLadders(string beginWord, string endWord, vector<string> &wordList) {
    vector<vector<string>> res;
    unordered_set<string> dict(wordList.begin(), wordList.end());
    if (!dict.count(endWord)) return res;

    // 使用双向BFS思想优化
    unordered_map<string, vector<string>> children;
    bool found = bfs(beginWord, endWord, dict, children);

    if (found) {
      vector<string> path{beginWord};
      dfs(children, path, beginWord, endWord, res);
    }

    return res;
  }

 private:
  bool bfs(string beginWord, const string &endWord, unordered_set<string> &dict,
           unordered_map<string, vector<string>> &children) {
    unordered_set<string> forward{beginWord};
    unordered_set<string> backward{endWord};
    bool found = false;
    bool reverse = false;

    // 移除起始词，因为它不需要在字典中
    dict.erase(beginWord);
    dict.erase(endWord);

    while (!forward.empty() && !backward.empty()) {
      // 总是从较小的集合开始扩展
      if (forward.size() > backward.size()) {
        swap(forward, backward);
        reverse = !reverse;
      }

      unordered_set<string> nextLevel;

      // 标记这一层访问过的单词，防止同层重复扩展
      unordered_set<string> visitedInThisLevel;

      for (const string &word : forward) {
        string newWord = word;
        for (int i = 0; i < word.length(); i++) {
          char originalChar = newWord[i];
          for (char c = 'a'; c <= 'z'; c++) {
            if (c == originalChar) continue;
            newWord[i] = c;

            string parent = reverse ? newWord : word;
            string child = reverse ? word : newWord;

            if (backward.count(newWord)) {
              children[parent].push_back(child);
              found = true;
            } else if (dict.count(newWord) && !found) {
              children[parent].push_back(child);
              nextLevel.insert(newWord);
              visitedInThisLevel.insert(newWord);
            }
          }
          newWord[i] = originalChar;
        }
      }

      // 从字典中移除这一层访问过的单词
      for (const string &word : visitedInThisLevel) {
        dict.erase(word);
      }

      if (found) break;
      forward = nextLevel;
    }

    return found;
  }

  void dfs(unordered_map<string, vector<string>> &children, vector<string> &path, const string &current,
           const string &endWord, vector<vector<string>> &res) {
    if (current == endWord) {
      res.push_back(path);
      return;
    }

    for (const string &child : children[current]) {
      path.push_back(child);
      dfs(children, path, child, endWord, res);
      path.pop_back();
    }
  }
};