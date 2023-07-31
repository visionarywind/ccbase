#include <iostream>
#include <tools/tool.h>
#include "inherit.h"

int main() {
  std::cout << "Hello world" << std::endl;
  Base::GetInstance().call();
  return 0;
}
