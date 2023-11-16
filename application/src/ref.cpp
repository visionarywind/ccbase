//
// Created by shanfeng on 12/10/2023.
//

#include "ref.h"

#include <atomic>
#include <iostream>

#include "pthread.h"

void *callback_thread_func(void *data) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
#ifdef WITH_BACKEND
  auto callback_thread = reinterpret_cast<CallbackThread *>(data);
  while (callback_thread->flag_.load()) {

  }
  MS_LOG(INFO) << "Exit callback thread loop.";
#endif
  return data;
}

struct Ref {
  ~Ref() { cancel(); }

  // pthread_cancel may cause bug now, so just set flag to false.
  void cancel() {
    if (flag_.load()) {
      flag_.store(false);
    }
  }

  int create() {
    flag_.store(true);
    return pthread_create(&thread_, NULL, &callback_thread_func, this);
  }

  pthread_t thread_;
  std::atomic_bool flag_{true};
  int32_t default_timeout_{100};
};

void AddRef::Add() {
    std::cout << "add ref" << std::endl;
    list_.emplace_back(std::make_shared<Ref>());
    std::cout << "ref size : " << list_.size() << std::endl;
}

void RefTest() {
  AddRef ref;
  ref.Add();
  ref.Add();
  ref.Add();
}