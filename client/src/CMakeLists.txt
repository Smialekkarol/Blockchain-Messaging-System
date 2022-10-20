set(USER_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(USER_SOURCES 
  ${USER_DIR}/Client.cpp
  ${USER_DIR}/App.cpp
)

add_executable(user ${USER_SOURCES})

target_link_libraries(user 
            boost_system
            pthread)
