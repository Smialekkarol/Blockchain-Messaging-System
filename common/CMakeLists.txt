set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(COMMON_SOURCES
  "${COMMON_DIR}/common.cpp"
)

add_library(common ${COMMON_SOURCES})

find_package(GTest CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(static_mock CONFIG REQUIRED)

set(libraries GTest::gtest GTest::gmock GTest::gtest_main static_mock::static_mock Threads::Threads)
target_include_directories(common PUBLIC ${libraries} ${static_mock_INCLUDE_DIRS})
target_link_libraries(common PUBLIC ${libraries})

include(GoogleTest)

target_include_directories(common PUBLIC .)

add_subdirectory(itf)
add_subdirectory(serialization)
add_subdirectory(utils)
