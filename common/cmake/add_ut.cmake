include(CMakeParseArguments)

if(ENABLE_UNIT_TESTS)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

function(add_ut target_name)
  set(flags)
  set(args)
  set(listArgs SRC LIB)
  cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" ${ARGN})

  if(NOT arg_SRC)
    message(FATAL_ERROR "SRC is a required argument")
  endif(NOT arg_SRC)

  set(libraries GTest::GTest GTest::Main Threads::Threads ${arg_LIB})

  add_executable(${target_name} ${arg_SRC})
  target_compile_definitions(${target_name} PUBLIC -DRESOURCES_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/ut/${target_name}/")
  target_link_libraries(${target_name} ${libraries})
endfunction(add_ut)
else()
function(add_ut)
endfunction(add_ut)
endif()