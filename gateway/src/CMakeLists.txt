set(GATEWAY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GATEWAY_SOURCES 
  ${GATEWAY_DIR}/WebSocketServer.cpp
  ${GATEWAY_DIR}/App.cpp
)

add_executable(gateway ${GATEWAY_SOURCES})

target_link_libraries(gateway PRIVATE
            common
            boost_system
            pthread
            amqpcpp 
            ev)
