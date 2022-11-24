set(GATEWAY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GATEWAY_SOURCES 
  ${GATEWAY_DIR}/App.cpp
  ${GATEWAY_DIR}/GatewayAsync.cpp
  ${GATEWAY_DIR}/Listener.cpp
)

add_executable(gateway ${GATEWAY_SOURCES})

find_package(Boost 1.80.0 COMPONENTS serialization)
if(Boost_FOUND)
  target_include_directories(gateway PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(gateway PRIVATE ${Boost_LIBRARIES})
endif()

target_link_libraries(gateway PRIVATE
            common
            boost_system
            pthread
            amqpcpp 
            ev)
