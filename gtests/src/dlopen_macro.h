#pragma once

#include <iostream>
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include <string>
#include <functional>

#ifndef _WIN32
#define PORTABLE_EXPORT __attribute__((visibility("default")))
#else
#define PORTABLE_EXPORT __declspec(dllexport)
#endif

#define ORIGIN_METHOD(name, return_type, ...)                   \
  extern "C" {                                                  \
  return_type name(__VA_ARGS__);                                \
  }                                                             \
  constexpr const char *k##name##Name = #name;                  \
  using name##FunObj = std::function<return_type(__VA_ARGS__)>; \
  using name##FunPtr = return_type (*)(__VA_ARGS__);

inline static std::string GetDlErrorMsg() {
#ifndef _WIN32
  const char *result = dlerror();
  return (result == nullptr) ? "Unknown" : result;
#else
  return std::to_string(GetLastError());
#endif
}

template <class T>
static T DlsymWithCast(void *handle, const char *symbol_name) {
#ifndef _WIN32
  T symbol = reinterpret_cast<T>(reinterpret_cast<intptr_t>(dlsym(handle, symbol_name)));
#else
  T symbol = reinterpret_cast<T>(GetProcAddress(reinterpret_cast<HINSTANCE__ *>(handle), symbol_name));
#endif
  if (symbol == nullptr) {
    std::cout << "symbol " << symbol_name << " is nullptr" << std::endl;
  }
  return symbol;
}

#define DlsymFuncObj(func_name, plugin_handle) DlsymWithCast<func_name##FunPtr>(plugin_handle, k##func_name##Name);