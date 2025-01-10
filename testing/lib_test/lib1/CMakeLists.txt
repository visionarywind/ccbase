# 设置项目名称
project(lib1)

# 添加源文件
# add_library(lib1 SHARED lib_loader1.cc)
add_library(lib1 SHARED lib_loader1.cc $<TARGET_OBJECTS:lib>)
target_link_libraries(lib1 lib2)
add_dependencies(lib1 lib)

# target_link_libraries(lib1 lib)

# 设置动态链接库的输出路径（可选，这里设置到构建目录下的lib文件夹）
set_target_properties(lib1 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs)