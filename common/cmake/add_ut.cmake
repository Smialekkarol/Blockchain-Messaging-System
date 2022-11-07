include(CMakeParseArguments)

if(ENABLE_UNIT_TESTS)
  find_package(GTest CONFIG REQUIRED)
  find_package(Threads REQUIRED)
  find_package(static_mock CONFIG REQUIRED)

  if(static_mock_FOUND)
      include_directories(${static_mock_INCLUDE_DIRS})
  endif(static_mock_FOUND)

  include(GoogleTest)

  function(add_ut target_name)
    set(flags)
    set(args)
    set(listArgs SRC LIB ORIGS)
    cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" "${ORIGS}" ${ARGN})

    if(NOT arg_SRC)
      message(FATAL_ERROR "SRC is a required argument")
    endif(NOT arg_SRC)

    add_executable(${target_name} ${arg_SRC})

    foreach(original_class "${arg_ORIGS}")
      target_compile_definitions(${target_name} PUBLIC "${original_class}_orig")
    endforeach()

    set(libraries GTest::gtest GTest::gmock GTest::gtest_main static_mock::static_mock Threads::Threads ${arg_LIB})
    target_link_libraries(${target_name} PRIVATE ${libraries})

    gtest_discover_tests(${target_name})
  endfunction(add_ut)
else()

  function(add_ut)
  endfunction(add_ut)

endif()