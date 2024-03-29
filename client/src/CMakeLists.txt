file(COPY Nodes.yaml DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
set(USER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(USER_SOURCES 
  ${USER_DIR}/Client.cpp
  ${USER_DIR}/App.cpp
  ${USER_DIR}/Utils.cpp
  ${USER_DIR}/AmqpHandler.cpp
)

add_executable(user ${USER_SOURCES})

find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost 1.80.0 COMPONENTS serialization)
if(Boost_FOUND)
  target_include_directories(user PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(user PRIVATE ${Boost_LIBRARIES})
endif()


target_link_libraries(user PRIVATE
            common
            boost_system
            pthread
            amqpcpp 
            ev
            spdlog::spdlog
            ${YAML_CPP_LIBRARIES})

