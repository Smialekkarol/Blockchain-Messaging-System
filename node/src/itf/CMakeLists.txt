target_include_directories(node PUBLIC .)

list(APPEND ${NODE_SOURCES} Block.cpp Message.cpp Header.cpp)