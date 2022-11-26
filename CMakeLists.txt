cmake_minimum_required(VERSION 3.2.0)
project(TODO_CHANGE_IT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(COMMON_CMAKE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake")
set(CMAKE_MODULE_PATH ${COMMON_CMAKE_MODULES} ${CMAKE_MODULE_PATH})

if(ENABLE_UNIT_TESTS)
  include(CTest)
  enable_testing()
  add_compile_definitions(UNIT_TEST=1)
endif()

include(add_ut)

option(BUILD_ALL_COMPONENTS "Enable all components build" OFF)
option(BUILD_NODE "Enable node build" OFF)
option(BUILD_CLIENT "Enable client build" OFF)

set(REPO_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(common)
include_directories(.)

if(BUILD_NODE OR BUILD_ALL_COMPONENTS)
  add_subdirectory(node)
endif()


if(BUILD_CLIENT OR BUILD_ALL_COMPONENTS)
  add_subdirectory(client)
endif()
