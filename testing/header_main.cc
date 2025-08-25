#include "header.h"
#include <iostream>

int main() {
  List list;
  list.head = nullptr;
  list.tail = nullptr;
  list.Insert(1);
  list.Insert(2);
  list.Insert(3);
  list.Print();
  return 0;
}
