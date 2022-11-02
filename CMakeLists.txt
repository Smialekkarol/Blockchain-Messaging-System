cmake_minimum_required(VERSION 2.8)
project(TODO_CHANGE_IT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(COMMON_CMAKE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake")
set(CMAKE_MODULE_PATH ${COMMON_CMAKE_MODULES} ${CMAKE_MODULE_PATH})
include(add_ut)

option(BUILD_NODE "Enable node build" ON)
option(BUILD_GATEWAY "Enable gatewat build" ON)
option(BUILD_CLIENT "Enable client build" ON)

add_subdirectory(common)

if(BUILD_NODE)
  add_subdirectory(node)
endif()

if(BUILD_GATEWAY)
  add_subdirectory(gateway)
endif()

if(BUILD_CLIENT)
  add_subdirectory(client)
endif()