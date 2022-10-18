
add_executable (http-server-sync
    ${BOOST_BEAST_FILES}
    HTTPServer.cpp
)

target_link_libraries(http-server-sync
    /usr/lib/lib-asio
    lib-asio-ssl
    lib-beast
    lboost_system
    lboost_thread
    lpthread)

