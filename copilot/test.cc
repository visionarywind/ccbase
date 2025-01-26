#include <iostream>
using namespace std;

void test() {
  int *p = (int *)malloc(sizeof(int));
  *p = 1;
  cout << *p << endl;
  free(p);
}

int main() {
  test();
  return 0;
}