#include <iostream>

void functionA() {
  void *returnAddress = __builtin_return_address(0);
  std::cout << "Function A - Caller's return address: " << returnAddress << std::endl;
}

void functionB() {
  void *returnAddress = __builtin_return_address(1);
  std::cout << "Function B - Caller's return address: " << returnAddress << std::endl;
  functionA();
}

int main() {
  functionB();
  return 0;
}