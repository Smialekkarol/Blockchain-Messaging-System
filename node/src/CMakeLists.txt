set(NODE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NODE_SOURCES 
  ${NODE_DIR}/App.cpp
  ${NODE_DIR}/Consumer.cpp
  ${NODE_DIR}/Producer.cpp
  ${NODE_DIR}/Utils.cpp
  ${NODE_DIR}/ConfigurationReader.cpp
  ${NODE_DIR}/Buffer.cpp
)

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(Libev REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_path(HIREDIS_HEADER hiredis REQUIRED)
find_library(HIREDIS_LIB hiredis REQUIRED)
find_path(REDIS_PLUS_PLUS_HEADER sw REQUIRED)
find_library(REDIS_PLUS_PLUS_LIB redis++ REQUIRED)
find_package(Boost 1.80.0 COMPONENTS serialization REQUIRED)

if(NOT ENABLE_UNIT_TESTS)
  add_executable(node ${NODE_SOURCES})
  target_include_directories(node PRIVATE ${HIREDIS_HEADER})
  target_link_libraries(node PRIVATE ${HIREDIS_LIB})
  target_include_directories(node PRIVATE ${REDIS_PLUS_PLUS_HEADER})
  target_link_libraries(node PRIVATE ${REDIS_PLUS_PLUS_LIB})
  if(Boost_FOUND)
    target_include_directories(node PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(node PRIVATE ${Boost_LIBRARIES})
  endif()
  target_link_libraries(node PRIVATE common amqpcpp Threads::Threads ${LIBEV_LIBRARIES} ${YAML_CPP_LIBRARIES} spdlog::spdlog)
  target_include_directories(node PRIVATE .)
endif()

add_subdirectory(db)
add_subdirectory(slot)
add_subdirectory(ut)
