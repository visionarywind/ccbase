/**
 * 测试虚函数指针的位置
 *  测试结果：位于对象的起始位置
 */
#include <iostream>
using namespace std;

class __attribute__((packed)) A {
 public:
  int i;
};

class __attribute__((packed)) B {
 public:
  virtual void testfunc() {}
};

class __attribute__((packed)) C {
 public:
  int data;
  virtual void testfunc() {}
};


int main() {
  std::cout << sizeof(A) << std::endl;
  std::cout << sizeof(B) << std::endl;
  std::cout << sizeof(C) << std::endl;

  C c;
	char* p1 = reinterpret_cast<char*>(&c);
	char* p2 = reinterpret_cast<char*>(&(c.data));
	if (p1 == p2) {
		std::cout << "虚函数表指针位于对象内存的末尾" << std::endl;
	}
	else {
		std::cout << "虚函数表指针位于对象内存的开头" << std::endl;
	}
  return 0;
}