#include <iostream>
#include <jemalloc/jemalloc.h>
using namespace std;

// void *operator new(size_t size) {
//   cout << "new " << size << endl;
//   return std::malloc(size);
// }

void test() {
  cout << "init" << endl;
  int *p = (int *)je_malloc(sizeof(int));
  cout << "ptr : " << p << endl;
  free(p);
}

int main() {
  test();
  return 0;
}