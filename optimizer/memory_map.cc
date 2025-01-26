#include <iostream>
#include <utility>
#include <functional>
#include <stdexcept>

#include <mutex>

template <typename Key, typename Value, std::size_t Capacity = 100000000>
class Map {
 public:
  std::pair<Key, Value> data[Capacity];
  bool occupied[Capacity] = {false};
  std::size_t map_size = 0;

  std::size_t hash(const Key &key) const { return std::hash<Key>()(key) % Capacity; }

  std::size_t find_index(const Key &key) const {
    std::size_t index = hash(key);
    std::size_t original_index = index;
    do {
      if (!occupied[index] || (occupied[index] && data[index].first == key)) {
        return index;
      }
      index = (index + 1) % Capacity;
    } while (index != original_index);
    return Capacity;
  }

 public:
  Map() = default;

  ~Map() = default;

  void insert(const Key &key, const Value &value) {
    if (map_size >= Capacity) {
      throw std::overflow_error("Map is full");
    }
    std::size_t index = find_index(key);
    if (!occupied[index]) {
      occupied[index] = true;
      map_size++;
    }
    data[index] = std::make_pair(key, value);
  }

  Value &at(const Key &key) {
    std::size_t index = find_index(key);
    if (index == Capacity || !occupied[index]) {
      throw std::out_of_range("Key not found");
    }
    return data[index].second;
  }

  bool erase(const Key &key) {
    std::size_t index = find_index(key);
    if (index == Capacity || !occupied[index]) {
      return false;
    }
    occupied[index] = false;
    map_size--;
    return true;
  }

  std::size_t size() const { return map_size; }

  bool empty() const { return map_size == 0; }

  void clear() {
    for (std::size_t i = 0; i < Capacity; ++i) {
      occupied[i] = false;
    }
    map_size = 0;
  }
};
Map<void *, size_t> memoryMap;

// 用于统计分配的总内存大小
std::size_t totalAllocated = 0;
std::size_t peakAllocated = 0;
// 用于统计当前已分配的内存大小
std::size_t currentAllocated = 0;
std::size_t allocCount = 0;
std::mutex memoryMutex;

// 全局重载operator new
void *operator new(std::size_t size) {
  std::lock_guard<std::mutex> lock(memoryMutex);
  void *ptr = std::malloc(size);
  if (!ptr) {
    throw std::bad_alloc();
  }
  allocCount++;
  totalAllocated += size;
  currentAllocated += size;
  peakAllocated = std::max(peakAllocated, currentAllocated);
  // if (allocCount % 10000 == 0) {
  std::cout << "peakAllocated : " << peakAllocated << ", currentAllocated : " << currentAllocated << ", ptr : " << ptr
            << std::endl;
  // }
  memoryMap.insert(ptr, size);
  return ptr;
}

// 全局重载operator delete
void operator delete(void *ptr) noexcept {
  std::cout << "free : " << ptr << std::endl;
  std::lock_guard<std::mutex> lock(memoryMutex);
  if (ptr) {
    size_t index = memoryMap.find_index(ptr);
    if (index == 100000000) {
      std::cout << "free failed : " << ptr << std::endl;
      std::free(ptr);
      return;
    }

    const auto &size = memoryMap.at(ptr);
    currentAllocated -= size;
    memoryMap.erase(ptr);
    std::free(ptr);
  } else {
    std::cout << "free failed : " << ptr << std::endl;
  }
}

struct Node {
  int val;
  Node(int v) : val(v) {}
  void *ptr;
};
int main() {
  Map<std::string, int, 10> myMap;
  myMap.insert("one", 1);
  myMap.insert("two", 2);
  myMap.insert("three", 3);
  auto node = new Node(1);
  std::cout << node->val << std::endl;

  std::cout << "Size of map: " << myMap.size() << std::endl;
  std::cout << "Value at 'two': " << myMap.at("two") << std::endl;

  myMap.erase("two");
  std::cout << "Size of map after erasing 'two': " << myMap.size() << std::endl;

  try {
    std::cout << "Value at 'two': " << myMap.at("two") << std::endl;
  } catch (const std::out_of_range &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}