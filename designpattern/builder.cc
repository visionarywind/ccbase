#include <iostream>
#include <functional>
using namespace std;

struct ComplexObj {
  ComplexObj(int a, int b, int c) : a_(a), b_(b), c_(c) {}
  int a_;
  int b_;
  int c_;
  function<int(int)> printer_;
};

struct ComplexObjBuilder final {
  ComplexObjBuilder() = default;
  ~ComplexObjBuilder() = default;

  ComplexObjBuilder(const ComplexObjBuilder &) = delete;
  ComplexObjBuilder &operator=(const ComplexObjBuilder &) = delete;
  ComplexObjBuilder(ComplexObjBuilder &&) = default;
  ComplexObjBuilder &operator=(ComplexObjBuilder &&) = default;

  ComplexObjBuilder &&set_a(int a) && {
    a_ = a;
    return std::move(*this);
  }

  ComplexObjBuilder &&set_b(int b) && {
    b_ = b;
    return std::move(*this);
  }
  ComplexObjBuilder &&set_c(int c) && {
    c_ = c;
    return std::move(*this);
  }

  ComplexObjBuilder &&set_printer(function<int(int)> printer) && {
    printer_ = [&](int n) {
      std::cout << a_ << ", " << b_ << ", " << c_ << std::endl;
      return printer(n);
    };
    return std::move(*this);
  }

  ComplexObj *build() && { return new ComplexObj(a_, b_, c_); }

  int a_{-1};
  int b_{-1};
  int c_{-1};
  function<int(int)> printer_{[&](int n) {
    std::cout << a_ << ", " << b_ << ", " << c_ << std::endl;
    return n;
  }};
};

int main() {
  auto obj = ComplexObjBuilder().set_printer([](int x) { return x; }).set_a(0).set_b(1).set_c(2).build();
  std::cout << obj->a_ << "," << obj->b_ << "," << obj->c_ << std::endl;
  delete obj;
  return 0;
}