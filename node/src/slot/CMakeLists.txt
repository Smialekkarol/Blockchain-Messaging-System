if(NOT ENABLE_UNIT_TESTS)
    set(SLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(SLOT_SOURCES 
        ${SLOT_DIR}/SlotHandler.cpp
        ${SLOT_DIR}/PendingBlockSaver.cpp
        ${SLOT_DIR}/ContributionNotifier.cpp
        ${SLOT_DIR}/InspectionWaiter.cpp
        ${SLOT_DIR}/ElectionNotifier.cpp
        ${SLOT_DIR}/ElectionWaiter.cpp
        ${SLOT_DIR}/PendingBlockRemover.cpp
        ${SLOT_DIR}/CompleteBlockSaver.cpp
    )
    target_include_directories(node PRIVATE .)
    target_sources(node PRIVATE ${SLOT_SOURCES})
endif()

add_subdirectory(ut)
