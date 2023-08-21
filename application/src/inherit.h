#pragma once

#include <iostream>

extern int gBase;

class Base;
static Base *GetBase();

class Base {
 public:
  virtual void call() = 0;
  static Base *GetInstance() { return GetBase(); }
};

class Derived : public Base {
 public:
  void call() override;
};

class DerivedX : public Base {
 public:
  void call() override;
};

static Base *GetBase() {
  static Base *base = gBase > 0 ? (Base *)(new DerivedX) : (Base *)(new Derived);
  return base;
}

class Fruit;
static Fruit &GetFruit();

class Fruit {
 public:
  static Fruit &GetInstance() { return GetFruit(); }
  virtual std::string name() { return "Fruit"; }
};

class Apple : public Fruit {
 public:
  std::string name() override { return "Apple"; }
};

static Fruit &GetFruit() {
  static Fruit base = gBase > 0 ? Apple{} : Fruit{};
  return base;
}

void base_test();