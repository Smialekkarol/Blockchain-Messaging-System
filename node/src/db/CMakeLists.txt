if(NOT ENABLE_UNIT_TESTS)
    set(DB_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(DB_SOURCES 
        ${DB_DIR}/RedisDB.cpp
    )

    target_include_directories(node PRIVATE .)
    target_sources(node PRIVATE ${DB_SOURCES})
endif()
