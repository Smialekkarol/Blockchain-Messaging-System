set(ITF_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ITF_SOURCES 
  ${ITF_DIR}/Block.cpp
  ${ITF_DIR}/Message.cpp
  ${ITF_DIR}/Header.cpp
)

target_include_directories(common PUBLIC .)
target_sources(common PUBLIC ${ITF_SOURCES})