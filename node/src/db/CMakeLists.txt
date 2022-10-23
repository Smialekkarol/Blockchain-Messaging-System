cmake_minimum_required(VERSION 3.0.0)
project(redis VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

add_executable(redis tmp_snapshot.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(redis PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(redis PUBLIC ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(redis PRIVATE ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(redis PRIVATE ${REDIS_PLUS_PLUS_LIB})

find_package(Boost 1.80.0 COMPONENTS serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
#   target_include_directories(redis PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(redis PRIVATE ${Boost_LIBRARIES})
endif()

target_include_directories(node PUBLIC .)