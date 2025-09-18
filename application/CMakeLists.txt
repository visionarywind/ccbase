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

find_package(robin_hood QUIET)
if(NOT robin_hood_FOUND)
	FetchContent_Declare(robin_hood
		GIT_REPOSITORY https://gitee.com/mirrors/robin-hood-hashing.git
		GIT_TAG 3.11.5
	)
  FetchContent_MakeAvailable(robin_hood)
endif()

find_package(json QUIET)
if(NOT json_FOUND)
	FetchContent_Declare(json
		GIT_REPOSITORY https://gitee.com/mirrors/nlohmann-json.git
		GIT_TAG v3.11.3
	)
	FetchContent_MakeAvailable(json)
endif()

find_package(abseil QUIET)
if(NOT abseil_FOUND)
	FetchContent_Declare(abseil
		GIT_REPOSITORY https://gitee.com/mirrors/abseil-cpp.git
		GIT_TAG 20250814.rc1
	)
	FetchContent_MakeAvailable(abseil)
endif()

add_executable(application ${SOURCE_FILES})
target_link_libraries(application LINK_PRIVATE tools common robin_hood nlohmann_json::nlohmann_json absl::base absl::strings absl::btree)
