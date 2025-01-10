#pragma once

extern "C" __attribute__((visibility("default"))) void load1();


class __attribute__((visibility("default")))  MyClass1 : public MyClass {
 public:
    MyClass1(int value) {
        value_ = value;
    }
public:
  int getValue() override {
    return value_;
  }

  void setValue(int num) override {
    value_ = num;
  }
 protected:
    int value_;
};

extern "C" __attribute__((visibility("default"))) MyClass* load_1();