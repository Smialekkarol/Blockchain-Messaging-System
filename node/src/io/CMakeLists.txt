if(NOT ENABLE_UNIT_TESTS)
    set(IO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(IO_SOURCES 
        ${IO_DIR}/ConnectionData.cpp
        ${IO_DIR}/ConnectionStore.cpp
        ${IO_DIR}/Channel.cpp
        ${IO_DIR}/ChannelStore.cpp
    )
    target_include_directories(node PRIVATE .)
    target_sources(node PRIVATE ${IO_SOURCES})
endif()
