#include <Python.h>

int main(int argc, char *argv[]) {
    // 初始化 Python 解释器
    Py_Initialize();

    // 检查 Python 解释器是否成功初始化
    if (!Py_IsInitialized()) {
        fprintf(stderr, "Failed to initialize Python interpreter\n");
        return 1;
    }

    // 执行 Python 脚本
    FILE* file = fopen("hello.py", "r");
    if (file != nullptr) {
        // 执行文件中的 Python 代码
        PyRun_SimpleFile(file, "hello.py");
        fclose(file);
    } else {
        fprintf(stderr, "Failed to open Python script\n");
    }

    // 关闭 Python 解释器
    Py_Finalize();

    return 0;
}