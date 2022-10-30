set(USER_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(USER_SOURCES 
  ${USER_DIR}/Client.cpp
  ${USER_DIR}/App.cpp
  /root/node/src/itf/Message.cpp
)

add_executable(user ${USER_SOURCES})

find_package(Boost 1.80.0 COMPONENTS serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})

endif()


target_link_libraries(user 
            boost_system
            pthread
            ${Boost_LIBRARIES})
