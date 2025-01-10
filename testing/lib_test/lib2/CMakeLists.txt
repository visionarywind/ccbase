# 设置项目名称
project(lib2)

# 添加源文件
# add_library(lib2 SHARED lib_loader2.cc)
add_library(lib2 SHARED lib_loader2.cc  $<TARGET_OBJECTS:lib>)
# add_dependencies(lib2 lib)

# target_link_libraries(lib2 lib)

# 设置动态链接库的输出路径（可选，这里设置到构建目录下的lib文件夹）
set_target_properties(lib2 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs)