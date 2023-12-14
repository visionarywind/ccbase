project(application)

set(SOURCE_FILES src/main.cpp
        src/inherit.cpp
				src/ref.cpp
				src/template.cpp
				src/actor_hash.cpp
				../nice/include/hqueue.h)

include(FetchContent)

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


# Configure for boost
#FetchContent_Declare(boost
#	GIT_REPOSITORY https://gitee.com/mirrors/boost.git
#	GIT_TAG boost-1.81
#	CONFIGURE_COMMAND ""
#	BUILD_COMMAND ""
#)

#FetchContent_GetProperties(boost)
#if(NOT boost)
#  FetchContent_Populate(boost)
#  add_subdirectory(${boost_SOURCE_DIR} ${boost_BINARY_DIR})
#endif()
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/robin_hood/include)

add_executable(application ${SOURCE_FILES})
target_link_libraries(application LINK_PRIVATE tools common ${libRobinHood})
