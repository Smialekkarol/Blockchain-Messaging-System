set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(COMMON_SOURCES
  "${COMMON_DIR}/common.cpp"
)

add_library(common ${COMMON_SOURCES})

target_include_directories(common PUBLIC "${REPO_ROOT_DIR}")

add_subdirectory(utils)
add_subdirectory(itf)
add_subdirectory(serialization)