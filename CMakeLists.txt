cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")
add_definitions("-std=c++17")

########################################################################
# RabbitMQ
include(ExternalProject)
set(AMQPCPP_ROOT "${CMAKE_CURRENT_BINARY_DIR}/amqpcpp")
ExternalProject_Add(libamqpcpp
    URL "https://github.com/hoxnox/AMQP-CPP/archive/v2.5.1-nx2.tar.gz"
    URL_MD5 "fcfbd25c03eecde4e4b0dfa58598a426"
    CMAKE_ARGS -Wno-dev "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    INSTALL_DIR ${AMQPCPP_ROOT}
)
set(AMQPCPP_INCLUDE_DIR ${AMQPCPP_ROOT}/include)
set(AMQPCPP_LIBRARIES ${AMQPCPP_ROOT}/lib/libamqp-cpp.a)
message(STATUS "Found external dependency AMQP-CPP: " ${AMQPCPP_INCLUDE_DIR})
include_directories(${AMQPCPP_INCLUDE_DIR})
list(APPEND LIBRARIES ${AMQPCPP_LIBRARIES})
list(APPEND DEPENDENCIES libamqpcpp)

find_package(Threads)
list(APPEND LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
list(APPEND LIBRARIES ev)

set(APP_SOURCES App.cpp Consumer.cpp Producer.cpp Utils.cpp)
add_executable(app ${APP_SOURCES})
target_link_libraries(app ${LIBRARIES})
add_dependencies(app ${DEPENDENCIES})

