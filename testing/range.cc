#include <iostream>
#include <set>
using namespace std;


struct Range {
  std::set<std::pair<size_t, size_t>> intervals_;

  void Mark(size_t start, size_t end) {
    if (start > end) {
      return;
    }

    auto new_interval = std::make_pair(start, end);
    auto it = intervals_.lower_bound(new_interval);
    size_t new_start = start;
    size_t new_end = end;
    if (it != intervals_.begin()) {
      --it;
      if (it->second < start) {
        ++it;
      }
    }
    while (it != intervals_.end() && it->first <= new_end) {
      new_start = std::min(new_start, it->first);
      new_end = std::max(new_end, it->second);
      it = intervals_.erase(it);
    }

    // std::cout << "Mark(" << start << ", " << end << ")" << std::endl;
    intervals_.insert(it, std::make_pair(new_start, new_end));
    // std::cout << "Result: " << std::endl;
    // for (auto& interval : intervals_) {
    //   std::cout << "[" << interval.first << ", " << interval.second << "]"
    //             << std::endl;
    // }
  }

  bool Query(size_t index) const {
    if (intervals_.empty()) {
      return false;
    }

    auto it = intervals_.lower_bound(std::make_pair(index, index));
    if (it != intervals_.end()) {
      if (it->first <= index && index <= it->second) {
        return true;
      }
    }
    if (it != intervals_.begin()) {
      --it;
      if (it->first <= index && index <= it->second) {
        return true;
      }
    }

    return false;
  }
};


int main() {
  Range range;
  range.Mark(1, 10);
  for (size_t i = 0; i < 20; ++i) {
    std::cout << range.Query(i) << " ";
  }
  std::cout << std::endl;

  range.Mark(0, 16);
  for (size_t i = 0; i < 20; ++i) {
    std::cout << range.Query(i) << " ";
  }
  std::cout << std::endl;

  range.Mark(19, 20);
  for (size_t i = 0; i < 20; ++i) {
    std::cout << range.Query(i) << " ";
  }
  std::cout << std::endl;
  return 0;
}