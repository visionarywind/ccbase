#pragma once

#ifndef MY_CLASS_DLL_H
#define MY_CLASS_DLL_H

#ifdef _WIN32
// 如果是Windows平台，使用__declspec(dllexport)来导出类，在客户端使用时会被解析为__declspec(dllimport)
#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
// 对于非Windows平台，可以使用其他合适的方式定义导出（比如Linux下用__attribute__((visibility("default"))) 等，这里暂不详细展开）
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

// 定义要导出的类
class DLL_EXPORT MyClass {
public:
    virtual int getValue() = 0;
    virtual void setValue(int num) = 0;
};

extern "C" {
    MyClass* generate();
}

#endif