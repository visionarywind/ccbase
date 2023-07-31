#include "inherit.h"

int gBase = 0;

void Derived::call() { std::cout << "Derived call" << std::endl; }

void DerivedX::call() { std::cout << "DerivedX call" << std::endl; };