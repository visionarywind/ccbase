# 设置项目名称
project(lib_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")


# 添加子目录，先构建动态链接库
add_subdirectory(lib)
add_subdirectory(lib1)
add_subdirectory(lib2)

# 再构建使用动态链接库的可执行文件
add_subdirectory(src)

