#pragma once
#include <iostream>
int gBase = 0;
class Base {
 public:
  static Base *GetInstance() {
    static auto *base = gBase > 0 : new DerivedX : new Derived;
    return base;
  }

  virtual void call() = 0;
};

class Derived : public Base {
 public:
  void call() override;
};

class DerivedX : public Base {
 public:
  void call() override;
};