# cmake_minimum_required(VERSION 3.0.0)
# project(redis VERSION 0.1.0)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include(CTest)
# enable_testing()

# add_executable(redis tmp_snapshot.cpp)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

target_include_directories(node PUBLIC .)

list(APPEND ${NODE_SOURCES} RedisDB.cpp)
