#include "inherit.h"

void base_test() {
  std::cout << "Base test" << std::endl;
  Base::GetInstance()->call();
  std::cout << "Fruit : " << Fruit::GetInstance().name() << std::endl;
}

int gBase = 0;

void Derived::call() { std::cout << "Derived call" << std::endl; }

void DerivedX::call() { std::cout << "DerivedX call" << std::endl; };