#include <iostream>
#include <thread>

class MyClass {
 public:
  void *operator new(size_t size) {
    std::cout << "Custom operator new called, size : " << size << std::endl;
    return malloc(size);
  }

  void operator delete(void *ptr) {
    std::cout << "Custom operator delete called." << std::endl;
    free(ptr);
  }

 public:
  virtual int getValue() { return 0; }

  void test() {
    std::cout << "test" << std::endl;
    // int *c = new int;
    // delete c;
  }

  std::vector<int> c;
};

class DerivedClass : public MyClass {
 public:
  int getValue() override { return value_; }
  int value_;
};

class App : public DerivedClass {
 public:
  int getValue() override { return app_value_; }
  int app_value_;
};

void threadFunction() {
  auto a = std::make_shared<App>();
  a->test();
  int *p = new int;
  delete p;
}

int main() {
  std::thread t(threadFunction);
  t.join();

  int *q = new int;
  delete q;

  return 0;
}