#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <functional>
#include <stdexcept>

template <typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>> {
 public:
  static std::shared_ptr<ObjectPool> Create(size_t initialSize = 0) {
    auto pool = std::shared_ptr<ObjectPool>(new ObjectPool());
    pool->Initialize(initialSize);
    return pool;
  }

  std::shared_ptr<T> Acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (object_cache_.empty()) {
      if (is_destroyed_) {
        throw std::runtime_error("ObjectPool has been destroyed");
      }

      std::cout << "Creating new object\n";
      return CreateNew();
    } else {
      std::cout << "Reusing existing object\n";
      auto ptr = object_cache_.front();
      object_cache_.pop();

      return std::shared_ptr<T>(ptr, [weak_pool = this->weak_from_this()](T *obj) {
        if (auto pool_ptr = weak_pool.lock()) {
          pool_ptr->Release(obj);
        } else {
          std::cout << "ObjectPool destroyed - deleting object\n";
          delete obj;
        }
      });
    }
  }

  void Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Clearing pool (" << object_cache_.size() << " objects)\n";
    while (!object_cache_.empty()) {
      delete object_cache_.front();
      object_cache_.pop();
    }
  }

  size_t Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return object_cache_.size();
  }

  void Destroy() {
    Clear();
    is_destroyed_ = true;
  }

  ~ObjectPool() {
    if (!is_destroyed_) {
      std::cout << "ObjectPool destructor called - clearing remaining objects\n";
      Clear();
    }
  }

 private:
  ObjectPool() = default;

  void Initialize(size_t initialSize) {
    for (size_t i = 0; i < initialSize; ++i) {
      object_cache_.push(CreateRaw());
    }
  }

  ObjectPool(const ObjectPool &) = delete;
  ObjectPool &operator=(const ObjectPool &) = delete;

  virtual T *CreateRaw() { return new T(); }

  std::shared_ptr<T> CreateNew() {
    T *raw_ptr = CreateRaw();
    return std::shared_ptr<T>(raw_ptr, [weak_pool = this->weak_from_this()](T *obj) {
      if (auto pool_ptr = weak_pool.lock()) {
        pool_ptr->Release(obj);
      } else {
        std::cout << "ObjectPool destroyed - deleting object\n";
        delete obj;
      }
    });
  }

  void Release(T *obj) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_destroyed_) {
      std::cout << "Pool destroyed - deleting released object\n";
      delete obj;
    } else {
      if constexpr (std::is_member_function_pointer_v<decltype(&T::Reset)>) {
        obj->Reset();
      }
      object_cache_.push(obj);
    }
  }

  std::queue<T *> object_cache_;
  mutable std::mutex mutex_;
  bool is_destroyed_ = false;
};

class MyResource {
 public:
  void Reset() {}
};

int main() {
  auto pool = ObjectPool<MyResource>::Create();

  {
    auto obj1 = pool->Acquire();
    auto obj2 = pool->Acquire();

    obj1->Reset();
  }

  size_t poolSize = pool->Size();
  std::cout << "pool size : " << poolSize << std::endl;
  pool->Clear();

  std::cout << "pool size : " << poolSize << std::endl;
  return 0;
}