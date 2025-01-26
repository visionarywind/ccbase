#include "map.h"

#include <iostream>
#include <map>

// 示例用法
int main() {
  RobinHoodHashMap<std::string, int> map;

  std::map<std::string, int> expected;

  for (int i = 0; i < 10000; i++) {
    map.insert(std::to_string(i), i);
    expected[std::to_string(i)] = i;
    if (map.size() != expected.size()) {
      std::cout << "size is not equal" << std::endl;
    }
  }

  for (int i = 0; i < 10000; i++) {
    std::string key = std::to_string(i);
    int *value = map.find(key);
    if (value == nullptr) {
      std::cout << "value is nullptr, key : " << key << std::endl;
    } else {
      if (*value != expected[key]) {
        std::cout << "value is not equal, key : " << key << std::endl;
      }
    }
  }

  return 0;
}