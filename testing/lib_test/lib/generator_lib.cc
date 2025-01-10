#include "generator_lib.h"

static int kStaticData = 0;
int kData = 0;

class MyClass1 : public MyClass {
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

MyClass * generate() {
  kStaticData++;
  kData++;
  return new MyClass1(kStaticData + kData);
}
