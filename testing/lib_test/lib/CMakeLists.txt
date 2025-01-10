# 设置项目名称
project(lib)

# 添加源文件
add_library(lib OBJECT generator_lib.cc)

# 设置动态链接库的输出路径（可选，这里设置到构建目录下的lib文件夹）
set_target_properties(lib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs)