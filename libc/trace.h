#pragma once
#include <string>

struct Event {
  std::string thread_name_;
  unsigned long thread_id_;
  void *ptr_;
  size_t size_;
};