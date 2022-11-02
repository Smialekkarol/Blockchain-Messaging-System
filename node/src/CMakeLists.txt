set(NODE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NODE_SOURCES 
  ${NODE_DIR}/App.cpp
  ${NODE_DIR}/Consumer.cpp
  ${NODE_DIR}/Producer.cpp
  ${NODE_DIR}/Utils.cpp
  ${NODE_DIR}/ConfigurationReader.cpp
)

add_executable(node ${NODE_SOURCES})

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(Libev REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)

find_path(HIREDIS_HEADER hiredis REQUIRED)
target_include_directories(node PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis REQUIRED)
target_link_libraries(node PUBLIC ${HIREDIS_LIB})

find_path(REDIS_PLUS_PLUS_HEADER sw REQUIRED)
target_include_directories(node PRIVATE ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++ REQUIRED)
target_link_libraries(node PRIVATE ${REDIS_PLUS_PLUS_LIB})

find_package(Boost 1.80.0 COMPONENTS serialization REQUIRED)
if(Boost_FOUND)
  target_include_directories(node PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(node PRIVATE ${Boost_LIBRARIES})
endif()

add_ut(TestConfigurationReader
  SRC ut/TestConfigurationReader.cpp
  LIB ${YAML_CPP_LIBRARIES} spdlog::spdlog
)

target_link_libraries(node PRIVATE amqpcpp Threads::Threads ${LIBEV_LIBRARIES} ${YAML_CPP_LIBRARIES} spdlog::spdlog)
target_include_directories(node PUBLIC .)

add_subdirectory(common)
add_subdirectory(itf)
add_subdirectory(serialization)
add_subdirectory(db)
