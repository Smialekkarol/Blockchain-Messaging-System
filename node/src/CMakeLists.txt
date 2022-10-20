set(NODE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NODE_SOURCES 
  ${NODE_DIR}/App.cpp
  ${NODE_DIR}/Consumer.cpp
  ${NODE_DIR}/Producer.cpp
  ${NODE_DIR}/Utils.cpp
  ${NODE_DIR}/ConfigurationReader.cpp
)
include_directories(${NODE_DIR})

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(Libev REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)

add_ut(TestConfigurationReader
  SRC ut/TestConfigurationReader.cpp
  LIB ${YAML_CPP_LIBRARIES} spdlog::spdlog
  )

add_executable(node ${NODE_SOURCES})
target_link_libraries(node amqpcpp Threads::Threads ${LIBEV_LIBRARIES} ${YAML_CPP_LIBRARIES} spdlog::spdlog)
