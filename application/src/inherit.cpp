#include "inherit.h"

void Derived call() override { std::cout << "Derived call" << std::end; }

void DerivedX::call() override { std::cout << "DerivedX call" << std::endl; };