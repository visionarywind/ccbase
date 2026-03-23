#pragma once
#include <iostream>

const size_t OBJ_SIZE = 1024;

class FixedPool {
 public:
  void *Alloc();
  void Free(void *ptr);
 private:
  struct Buf {
    struct Buf *next_;
  };
  struct Buffer {
   private:
    uint8_t data_[OBJ_SIZE * OBJ_SIZE];
  };
};