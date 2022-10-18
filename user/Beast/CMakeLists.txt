cmake_minimum_required(VERSION 3.8...3.20)

project(boost_beast VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES CXX)

add_library(boost_beast INTERFACE)
add_library(Boost::beast ALIAS boost_beast)

target_include_directories(boost_beast INTERFACE include)

target_link_libraries(boost_beast
  INTERFACE
    Boost::asio
    Boost::assert
    Boost::bind
    Boost::config
    Boost::container
    Boost::core
    Boost::endian
    Boost::intrusive
    Boost::logic
    Boost::mp11
    Boost::optional
    Boost::smart_ptr
    Boost::static_assert
    Boost::static_string
    Boost::system
    Boost::throw_exception
    Boost::type_traits
    Boost::utility
    Boost::winapi
)

target_compile_features(boost_beast INTERFACE cxx_std_11)

file (GLOB_RECURSE BOOST_BEAST_FILES
    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.cpp
    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.ipp
)

add_subdirectory (HTTP)
add_subdirectory (WebSocket)