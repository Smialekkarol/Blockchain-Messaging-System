if(NOT ENABLE_UNIT_TESTS)
    target_include_directories(node PRIVATE .)
endif()

add_subdirectory(ut)
