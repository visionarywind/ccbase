#include <iostream>
#include <tools/tool.h>
#include "inherit.h"

int main() {
  std::cout << "Application start" << std::endl;
  base_test();
  int a = 1;
  void *t = &a;
  std::shared_ptr<void *> m = std::make_shared<void *>(t);
  std::cout << m << std::endl;
  m.reset();

  return 0;
}
