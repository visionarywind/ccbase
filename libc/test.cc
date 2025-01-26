#include <iostream>
#include <memory>
#include <vector>
using namespace std;

// void *operator new(size_t size) {
//   cout << "new " << size << endl;
//   return std::malloc(size);
// }

void test() {
  cout << "init" << endl;
  int *p = (int *)malloc(sizeof(int));
  cout << "ptr : " << p << endl;
  free(p);
  cout << "===again===" << endl;

  int *c = new int[1024 * 1024 * 1024];
  cout << "ptr : " << c << endl;
  *c = 1;
  c[1024] = 1;
  cout << *c << endl;
  delete[] c;
  cout << "exit" << endl;
  std::vector<int> arr(5);
  // 初始化元素
  for (int i = 0; i < 5; ++i) {
    arr[i] = i * 2;
  }

  // 输出元素
  for (int i = 0; i < 5; ++i) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;
}

int main() {
  test();
  return 0;
}