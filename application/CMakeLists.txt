project(application)

set(SOURCE_FILES src/main.cpp
        src/inherit.cpp
				src/ref.cpp
				src/template.cpp
				src/actor_hash.cpp
				src/sorted_vector.cpp
				src/mem_dynamic_allocator.cpp
				../nice/include/hqueue.h)

include(FetchContent)

# # Configure for boost
# FetchContent_Declare(boost
# 	GIT_REPOSITORY https://gitee.com/mirrors/boost.git
# 	GIT_TAG boost-1.84.0
# 	CONFIGURE_COMMAND ""
# 	BUILD_COMMAND ""
# )
# FetchContent_GetProperties(boost)
# if(NOT boost)
#   FetchContent_Populate(boost)
#   add_subdirectory(${boost_SOURCE_DIR} ${boost_BINARY_DIR})
# endif()

# # Configure for folly.
# FetchContent_Declare(
#   folly
#   GIT_REPOSITORY https://gitee.com/mirrors/folly.git
#   GIT_TAG v2024.03.25.00
# 	CONFIGURE_COMMAND ""
# 	BUILD_COMMAND ""
# )
# FetchContent_MakeAvailable(folly)

# Configure for robin hood
set(libRobinHood robin_hood)
FetchContent_Declare(${libRobinHood}
	GIT_REPOSITORY https://gitee.com/mirrors/robin-hood-hashing.git
	GIT_TAG 3.11.5
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
)

FetchContent_GetProperties(${libRobinHood})
if(NOT ${libRobinHood})
  FetchContent_Populate(${libRobinHood})
  add_subdirectory(${${libRobinHood}_SOURCE_DIR} ${${libRobinHood}_BINARY_DIR})
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/robin_hood/include)


FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

# Configure for folly.
FetchContent_Declare(
  abseil-cpp
  GIT_REPOSITORY https://gitee.com/mirrors/abseil-cpp.git
  GIT_TAG 20240116.1
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
)
FetchContent_MakeAvailable(json abseil-cpp)

add_executable(application ${SOURCE_FILES})
target_link_libraries(application LINK_PRIVATE tools common ${libRobinHood} nlohmann_json::nlohmann_json absl::base absl::strings)
