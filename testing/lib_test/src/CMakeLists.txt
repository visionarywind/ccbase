# 设置项目名称
project(src)

# # 查找动态链接库（这里假设动态链接库在构建目录下的lib文件夹中）
# find_library(MYLIB lib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../libs)

# 添加可执行文件
add_executable(src main.cc)

# 链接动态链接库到可执行文件
# target_link_libraries(use_mylib ${MYLIB})
target_link_libraries(src lib1 lib2)