set(NODE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/App.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Consumer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Producer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationReader.cpp
)

add_executable(node)

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(Libev REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)

find_path(HIREDIS_HEADER hiredis)
target_include_directories(node PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(node PUBLIC ${HIREDIS_LIB})

find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(node PRIVATE ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(node PRIVATE ${REDIS_PLUS_PLUS_LIB})

find_package(Boost 1.80.0 COMPONENTS serialization)
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

target_sources(node PRIVATE ${NODE_SOURCES})
