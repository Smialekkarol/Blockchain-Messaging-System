set(UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(UTILS_SOURCES)

if(NOT ENABLE_UNIT_TESTS)
  target_sources(
    common 
    PUBLIC
    ${UTILS_DIR}/Time.cpp
    ${UTILS_DIR}/Timestamp.cpp
    ${UTILS_DIR}/Text.cpp
  )
else()
  target_sources(
    common 
    PUBLIC
    ${UTILS_DIR}/Timestamp.cpp
    ${UTILS_DIR}/Text.cpp
  )
endif()

include_directories(common PUBLIC .)

add_subdirectory(ut)