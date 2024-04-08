#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

constexpr int LIST_LEVEL = 15;

struct Block {
  Block(void *addr, size_t size, int status) : addr_(addr), size_(size), status_(status) {}
  void *addr_;
  size_t size_;
  int status_;  // 0 : free, 1 : used, 2 : eager free.

  int stream_id_{0};
  struct Block *prev_{nullptr};
  struct Block *next_{nullptr};

  void Print() {
    std::cout
      << "Block[" << addr_ << "] size_ : " << size_ << ", status_ : "
      << status_
      // << ", prev_ : " << (prev_ ? prev_->addr_ : 0) << ", next_ : " << (next_ ? next_->addr_ : 0) << std::endl;
      << ", prev_ : " << prev_ << ", next_ : " << next_ << std::endl;
  }
};
using BlockRawPtr = struct Block *;

struct Node {
  Node(BlockRawPtr block) : block_(block) {}
  Node(){};
  ~Node() {}

  size_t size() { return block_->size_; }

  BlockRawPtr block_{nullptr};
  Node *nexts_[LIST_LEVEL]{nullptr};
};

class SkipList {
 public:
  SkipList() : head_(new Node()) {}
  ~SkipList() {
    Node *begin = head_->nexts_[0];
    while (begin != nullptr) {
      Node *to_delete = begin;
      begin = begin->nexts_[0];
      delete to_delete;
    }
    delete head_;
  }
  // Disable copy and assignment constructor.
  SkipList(const SkipList &other) = delete;
  SkipList &operator=(SkipList const &) = delete;

  // Traverse elements in list.
  void Traverse(std::function<void(BlockRawPtr)> func) {
    Node *begin = head_->nexts_[0];
    while (begin != nullptr) {
      func(begin->block_);
      begin = begin->nexts_[0];
    }
  }

  bool RemoveNode(Node *node, Node *next[]) {
    for (int i = 0; i < LIST_LEVEL && next[i]->nexts_[i] == node; i++) {
      next[i]->nexts_[i] = next[i]->nexts_[i]->nexts_[i];
    }
    // std::cout << "delete node : " << node << std::endl;
    delete node;

    size_--;
    return true;
  }

  // Get size of list.
  [[nodiscard]] size_t Size() const { return size_; }
  // Print elements in list.
  void Print() {
    Traverse([](BlockRawPtr block) -> void {
      std::cout << "block : " << block << ", size : " << block->size_ << ", addr : " << block->addr_ << std::endl;
    });
  }

  // Add element into list.
  void Insert(BlockRawPtr block) {
    // std::cout << "Insert block : " << block << ", size : " << block->size_ << std::endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(block->size_, next);
    Node *node = new Node(block);
    // std::cout << "new node : " << node << std::endl;
    for (int i = 0; i < LIST_LEVEL; i++) {
      node->nexts_[i] = next[i]->nexts_[i];
      next[i]->nexts_[i] = node;
      if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count() &
          1) {
        break;
      }
    }
    size_++;
  }

  // Remove element by key.
  bool Remove(BlockRawPtr block) {
    Node *next[LIST_LEVEL] = {0};
    Locate(block->size_, next);
    Node *node = next[0]->nexts_[0];
    bool found = false;
    // std::cout << "Remove block : " << block << ", size : " << block->size_ << ", node size : " << node->block_->size_
    //           << std::endl;
    while (node != nullptr && node->size() == block->size_) {
      if (node->block_ == block) {
        found = true;
        break;
      }
      node = node->nexts_[0];
      // if (node->block_ == nullptr) {
      //   std::cout << "unknown branch" << std::endl;
      // }
    }
    if (found) {
      RemoveNode(node, next);
    } else {
      std::cout << "remove block failed : " << block << ", node : " << node << ", node->size : " << node->size()
                << ", block size : " << block->size_ << std::endl;
    }

    return found;
  }

 public:
  // Locate position that less than key.
  void Locate(size_t size, Node *next[]) {
    Node *cur = head_;
    for (int i = LIST_LEVEL - 1; i >= 0; i--) {
      while (cur->nexts_[i] != nullptr && cur->nexts_[i]->size() < size) {
        cur = cur->nexts_[i];
      }
      next[i] = cur;
    }
  }

 private:
  Node *head_;

  size_t size_{0};
};

int main() {
  std::srand(std::time(nullptr));
  int random_value = std::rand();

  SkipList sk;
  set<BlockRawPtr> s;
  vector<BlockRawPtr> vec;
  for (int i = 0; i < 10000000; i++) {
    BlockRawPtr block = new Block(malloc(i), i, 0);
    sk.Insert(block);
    s.insert(block);
    auto it = vec.begin();
    if (i > 100) {
      it++;
    } else if (i > 1000) {
      it += 10;
    } else if (i > 10000) {
      it += 100;
    } else if (i > 100000) {
      it += 1000;
    }
    vec.insert(it, block);
    // vec.emplace_back(block);
  }
  //   for (int i = 0; i < 10; i++) {
  //     int index = rand() % vec.size();
  //     auto block = vec[index];
  //     cout << "s erase : " << s.erase(block) << ", remain : " << s.size() << endl;
  //     cout << "sk erase : " << sk.Remove(block) << ", remain : " << sk.Size() << endl;
  //   }
  for (auto block : vec) {
    if (!sk.Remove(block)) {
      cout << "remain : " << sk.Size() << endl;
    }
    free(block->addr_);
  }
  cout << "final - remain : " << sk.Size() << endl;
  return 0;
}