set(NODE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NODE_SOURCES 
  ${NODE_DIR}/App.cpp
  ${NODE_DIR}/Consumer.cpp
  ${NODE_DIR}/Producer.cpp
  ${NODE_DIR}/Utils.cpp
)

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(ev REQUIRED)

add_executable(node ${NODE_SOURCES})
target_link_libraries(node amqpcpp Threads ev)
