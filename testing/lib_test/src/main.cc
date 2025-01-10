#include <iostream>
#include "../lib/generator_lib.h"
#include "../lib1/lib_loader1.h"
#include "../lib2/lib_loader2.h"

int main() {
  std::cout << "调用动态链接库中函数load1的结果: " << std::endl;
  MyClass *c1 = load_1();
  std::cout << "c1 : " << c1 << " " << c1->getValue() << std::endl;
//   std::cout << "调用动态链接库中函数load2的结果: " << std::endl;
//   MyClass *c2 = load_2();

//   std::cout << "c2 : " << c2->getValue() << std::endl;
  return 0;
}