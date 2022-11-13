set(UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(UTILS_SOURCES 
  ${UTILS_DIR}/Timestamp.cpp
  ${UTILS_DIR}/Text.cpp
)

target_include_directories(common PUBLIC .)
target_sources(common PUBLIC ${UTILS_SOURCES})

add_subdirectory(ut)