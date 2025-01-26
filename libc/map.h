#pragma once

#include <functional>
#include <stdexcept>

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class RobinHoodHashMap {
 private:
  // 哈希表条目结构
  struct Entry {
    Key key;
    Value value;
    int32_t probe_distance;  // 从原始哈希位置的偏移
    bool occupied = false;   // 是否被占用

    Entry() : probe_distance(-1), occupied(false) {}
    Entry(const Key &k, const Value &v, int32_t d) : key(k), value(v), probe_distance(d), occupied(true) {}
  };

  Entry *table = nullptr;                // 手动管理的动态数组
  size_t capacity_ = 0;                  // 当前容量
  size_t size_ = 0;                      // 元素数量
  Hash hasher;                           // 哈希函数
  const double max_load_factor_ = 0.75;  // 扩容阈值

 public:
  // 构造函数
  explicit RobinHoodHashMap(size_t initial_capacity = 8) {
    if (initial_capacity == 0) {
      throw std::invalid_argument("Initial capacity must be positive.");
    }
    resize_table(initial_capacity);
  }

  // 析构函数：释放内存
  ~RobinHoodHashMap() { delete[] table; }

  // 禁止拷贝（简化实现）
  RobinHoodHashMap(const RobinHoodHashMap &) = delete;
  RobinHoodHashMap &operator=(const RobinHoodHashMap &) = delete;

  // 返回元素数量
  size_t size() const { return size_; }

  // 返回当前容量
  size_t capacity() const { return capacity_; }

  // 插入键值对
  void insert(const Key &key, const Value &value) {
    if (load_factor() >= max_load_factor_) {
      resize_table(capacity_ * 2);  // 触发扩容
    }

    Entry new_entry(key, value, 0);
    size_t idx = hash(key);
    int32_t current_probe = 0;

    while (true) {
      if (!table[idx].occupied) {
        // 插入新条目
        table[idx] = new_entry;
        size_++;
        return;
      }

      // 键已存在则更新值
      if (table[idx].key == key) {
        table[idx].value = value;
        return;
      }

      // Robinhood 策略：交换探测距离更大的条目
      if (table[idx].probe_distance < current_probe) {
        std::swap(table[idx], new_entry);
        std::swap(table[idx].probe_distance, current_probe);
      }

      idx = next_index(idx);
      current_probe++;
      new_entry.probe_distance = current_probe;
    }
  }

  // 查找键
  Value *find(const Key &key) {
    size_t idx = hash(key);
    int32_t current_probe = 0;

    while (table[idx].occupied) {
      if (table[idx].key == key) {
        return &table[idx].value;
      }

      // 提前终止条件
      if (table[idx].probe_distance < current_probe) {
        break;
      }

      idx = next_index(idx);
      current_probe++;
    }

    return nullptr;  // 未找到
  }

  // 删除键
  bool erase(const Key &key) {
    size_t idx = hash(key);
    int32_t current_probe = 0;

    while (table[idx].occupied) {
      if (table[idx].key == key) {
        // 标记为未占用
        table[idx].occupied = false;
        size_--;

        // 调整后续条目
        size_t next_idx = next_index(idx);
        while (table[next_idx].occupied && table[next_idx].probe_distance > 0) {
          table[idx] = table[next_idx];
          table[idx].probe_distance--;
          table[next_idx].occupied = false;
          idx = next_idx;
          next_idx = next_index(next_idx);
        }
        return true;
      }

      if (table[idx].probe_distance < current_probe) {
        break;
      }

      idx = next_index(idx);
      current_probe++;
    }

    return false;
  }

 private:
  // 计算哈希值
  size_t hash(const Key &key) const { return hasher(key) % capacity_; }

  // 获取下一个索引（环形数组）
  size_t next_index(size_t idx) const { return (idx + 1) % capacity_; }

  // 计算负载因子
  double load_factor() const { return static_cast<double>(size_) / capacity_; }

  // 调整哈希表大小（核心扩容逻辑）
  void resize_table(size_t new_capacity) {
    if (new_capacity <= capacity_) return;

    // 分配新内存
    Entry *new_table = new Entry[new_capacity];
    size_t old_capacity = capacity_;
    Entry *old_table = table;

    // 更新内部状态
    table = new_table;
    capacity_ = new_capacity;
    size_ = 0;

    // 迁移旧数据
    for (size_t i = 0; i < old_capacity; ++i) {
      if (old_table[i].occupied) {
        insert(old_table[i].key, old_table[i].value);
      }
    }

    // 释放旧内存
    delete[] old_table;
  }
};